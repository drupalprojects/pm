<?php

/**
 * @file
 * Icon handling by Drupal PM.
 */

/**
 * Provides an add icon for Project Management nodes.
 */
function pm_icon_add_node($node, $params = array()) {
  return pm_icon_add('node/add/' . $node->type, $params);
}

/**
 * Provides an edit icon for Project Management nodes.
 */
function pm_icon_edit_node($node, $params = array()) {
  return pm_icon_edit('node/' . $node->nid . '/edit', $params);
}

/**
 * Provides a delete icon for Project Management nodes.
 */
function pm_icon_delete_node($node, $params = array()) {
  return pm_icon_delete('node/' . $node->nid . '/delete', $params);
}

/**
 * Provides a Project Management add icon.
 */
function pm_icon_add($path, $params = array()) {
  $attributes = array('class' => array('popups-form'));
  return pm_icon_l('application_add', $path, t('Add'), $params, $attributes);
}

/**
 * Provides a Project Management edit icon.
 */
function pm_icon_edit($path, $params = array()) {
  $attributes = array('class' => array('popups-form'));
  return pm_icon_l('application_edit', $path, t('Edit'), $params, $attributes);
}

/**
 * Provides a Project Management delete icon.
 */
function pm_icon_delete($path, $params = array()) {
  $attributes = array('class' => array('popups-form'));
  return pm_icon_l('application_delete', $path, t('Delete'), $params, $attributes);
}

/**
 * Provides an icon link.
 */
function pm_icon_l($icon, $path, $title, $params = array(), $attributes = array()) {
  if (!drupal_valid_path($path)) {
    return '';
  }
  $icon = pm_icon($icon, $title);
  $attributes['title'] = $title;

  $query = '';

  if (array_key_exists('q', $params)) {
    $destination = $params['q'];
    unset($params['q']);
    $c = 0;
    if (array_key_exists('page', $params)) {
      $destination .= '?page=' . $params['page'];
      unset($params['page']);
      $c++;
    }
    if (array_key_exists('sort', $params)) {
      if ($c) {
        $destination .= '&';
      }
      else {
        $destination .= '?';
      }
      $destination .= 'sort=' . $params['sort'];
      unset($params['sort']);
      $c++;
    }
    if (array_key_exists('order', $params)) {
      if ($c) {
        $destination .= '&';
      }
      else {
        $destination .= '?';
      }
      $destination .= 'order=' . $params['order'];
      unset($params['order']);
      $c++;
    }
    $query .= 'destination=' . urlencode($destination);
    unset($params['destination']);
  }

  return l($icon, $path, array(
    'attributes' => $attributes,
    'html' => TRUE,
    'query' => _pm_icon_urlencode_helper($params),
  ));
}

/**
 * Encodes URL for icon links.
 */
function _pm_icon_urlencode_helper($params, $org_key = "") {
  $new_params = array();
  foreach ($params as $key => $value) {
    if (!empty($org_key)) {
      $new_key = $org_key . "[" . $key . "]";
    }
    else {
      $new_key = $key;
    }
    if (is_array($value)) {
      $new_params = array_merge(_pm_icon_urlencode_helper($value, $new_key), $new_params);
    }
    else {
      $new_params[$new_key] = urlencode($value);
    }
  }
  return $new_params;
}

/**
 * Provides a Project Management icon.
 */
function pm_icon($icon, $title) {
  if (variable_get('pm_icon', PM_ICON_SET_DEFAULT_BEHAVIOUR) !== PM_ICON_SET_NO_ICON) {
    $output = array(
      '#theme' => 'pm_icon',
      '#title' => $title,
      '#icon' => $icon,
    );
  }
  else {
    $output = array(
      '#theme' => 'pm_icon_none',
      '#title' => $title,
    );
  }
  return drupal_render($output);
}

/**
 * Forces a recache of Project Management icons.
 */
function pm_icon_recache() {
  $available = array();

  $dir = variable_get('pm_icons_path', drupal_get_path('module', 'pm') . '/icons');

  $files = scandir($dir);
  foreach ($files as $file) {
    $available[] = $file;
  }

  cache_set('pm:icons', $available, 'cache', CACHE_TEMPORARY);
  $available = cache_get('pm:icons');
  return $available;
}

/**
 * Provides a default icon to avoid missing icons.
 */
function pm_icon_default($icon, $title) {
  // For now, just return $title.
  // A future extension could be more intelligent using $icon.
  return $title;
}

/**
 * Set body class to use pm default icons if required.
 */
function pm_preprocess_html(&$vars) {
  $pm_icon = variable_get('pm_icon', PM_ICON_SET_DEFAULT_BEHAVIOUR);
  $font_awesome_enabled = module_exists('fontawesome');
  switch ($pm_icon) {
    case PM_ICON_SET_DEFAULT_BEHAVIOUR:
      $vars['classes_array'][] = ($font_awesome_enabled) ? "pm-use-fa-icons" : "pm-use-storm-icons";

      break;

    case PM_ICON_SET_FONTAWESOME:
      $vars['classes_array'][] = "pm-use-fa-icons";

      break;

    case PM_ICON_SET_STORM:
      $vars['classes_array'][] = "pm-use-storm-icons";

      break;

  }
}

/**
 * Helper to attach fontawesome icons.
 */
function pm_helper_get_fa_icon($original_key, $default_class = 'fa_wrench') {
  $context = array(
    'original_key' => $original_key,
    'default_class' => $default_class,
  );
  $key = str_replace(' ', '_', $original_key);
  $key = str_replace('-', '_', $key);
  $map = pm_icon_map();
  $class = isset($map[$key]) ? $map[$key] : $key;

  drupal_alter('pm_icon_classes', $class, $key, $context);

  return $class;
}

/**
 * Get the entire mapping of icon list that Drupal PM supports.
 */
function pm_icon_map() {
  return array(
    'pmconfiguration' => 'fa-gear';
    'pmexpenses' => 'fa-money';
    'pmnotes' => 'fa-file-o';
    'pmtimetrackings' => 'fa-clock-o';
    'pmtickets' => 'fa-ticket';
    'pmtasks' => 'fa-tasks';
    'pmprojects' => 'fa-cubes';
    'pmteams' => 'fa-users';
    'pmorganizations' => 'fa-institution';
    'pmissues' => 'fa-cube';
    'application_add' => 'fa-plus';
    'application_delete' => 'fa-remove pm-status-danger-onfocus';
    'application_edit' => 'fa-edit';
    // Issue type.
    'bug' => 'fa-bug';
    'task' => 'fa-tasks';
    'feature_request' => 'fa-ticket';
    'support' => 'fa-phone';
    // Project type.
    'development' => 'fa-flask';
    // Progress.
    'in_progress' => 'fa-play pm-status-info';
    'completed' => 'fa-check pm-status-success';
    'on_hold' => 'fa-pause pm-status-warning';
    'inserted' => 'fa-square pm-status-normal';
    // Priority.
    '1_low' => 'fa-circle pm-status-info';
    '2_normal' => 'fa-circle pm-status-success';
    '3_high' => 'fa-circle pm-status-warning';
    '4_urgent' => 'fa-circle pm-status-danger';
  );
}
