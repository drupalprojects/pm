<?php
/**
 * @file
 * Provides theme functions for Project Management modules.
 */

/**
 * Provides HTML for a Project Management dashboard.
 */
function theme_pm_dashboard($vars) {
  $link_blocks = $vars['links'];
  $content = '<div id="pmdashboard">';
  if (!empty($link_blocks)) {
    $content .= '<dl class="pmdashboard clear-block">';
    foreach ($link_blocks as $link_block_array) {
      $content .= '<div class="pmdashboard">';
      if (!empty($link_block_array) AND is_array($link_block_array)) {
        foreach ($link_block_array as $link_array) {
          if (!empty($link_array['theme'])) {
            $content .= theme($link_array['theme'], array('link_blocks' => $link_array));
          }
          else {
            $content .= theme('pm_dashboard_link', array('link_blocks' => $link_array));
          }
        }
      }
      $content .= '</div>';
    }
    $content .= '</dl>';
  }
  else {
    $content .= t('No dashboard links available');
  }
  $content .= '</div>';

  return $content;
}

/**
 * Provides HTML for the Project Management dashboard blocks.
 */
function theme_pm_dashboard_block($vars) {
  $link_blocks = $vars['links'];
  $content = '<div id="pmdashboard-block">';
  if (!empty($link_blocks)) {
    foreach ($link_blocks as $link_array) {
      if (!empty($link_array['theme'])) {
        $content .= theme($link_array['theme'], array('link_blocks' => $link_array));
      }
      else {
        $content .= theme('pm_dashboard_link', array('link_blocks' => $link_array));
      }
    }
  }
  else {
    // No links, hide block.
    return '';
  }
  $content .= '</div>';
  return $content;
}

/**
 * Implements hook_preprocess_pm_list_default().
 */
function pm_preprocess_pm_list_default(&$vars) {
  $vars['value'] = str_replace(' ', '&nbsp;', $vars['value']);
}

/**
 * Implements hook_preprocess_pm_icon().
 */
function pm_preprocess_pm_icon(&$vars) {
  $icon = check_plain($vars['icon']);
  $title = check_plain($vars['title']);
  $icon = str_replace(' ', '_', $icon);
  $vars['class'] = pm_helper_get_fa_icon($icon);
}

/**
 * Implements hook_preprocess_pm_icon_none().
 */
function pm_preprocess_pm_icon_none(&$vars) {
  $vars['title'] = check_plain($vars['title']);
}

/**
 * Implements hook_preprocess_theme().
 */
function pm_preprocess_pm_dashboard_link(&$vars) {
  $link_array = $vars['link_blocks'];
  $params = array();
  if (!empty($link_array['nid'])) {
    $params_key = $link_array['node_type'] . '_nid';
    $params['query'] = array($params_key => $link_array['nid']);
  }
  $vars['path'] = url($link_array['path'], $params);
  $vars['title'] = $link_array['title'];
  $vars['icon'] = NULL;
  $vars['extra_link'] = NULL;
  $vars['extra_link_icon'] = NULL;

  $vars['icon'] = pm_icon($link_array['icon'], '');
  // Add extra link if present.
  if (!empty($link_array['extra_link'])) {
    $vars['extra_link'] = $link_array['extra_link'];
    $vars['extra_link_icon'] = pm_icon('application_add', t('+Add'));
  }
}

/**
 * Theme the pm entry table as a sortable list of menu items.
 */
function theme_pm_menu_entry_table($form) {
  $form = reset($form);
  // For some reason form comes in an empty array...
  if(!empty($form)){
    $rows = array();
    // Take all the form elements and format theme for theme_table
    foreach (element_children($form) as $key) {
      if (isset($form[$key]['title'])) {
        // Add the table drag functionality
        drupal_add_tabledrag('widget-menu-table', 'order', 'sibling', 'menu-weight');
        // Add class to group weight fields for drag and drop.
        $form[$key]['weight']['#attributes']['class'] = array('menu-weight');
        // array to store row data
        $rows[] = array(
          'data' => array(
            '',
            drupal_render($form[$key]['weight']),
            drupal_render($form[$key]['title']),
            drupal_render($form[$key]['path']) .
            drupal_render($form[$key]['extra_link']),
            drupal_render($form[$key]['icon']),
            drupal_render($form[$key]['status']),
            drupal_render($form[$key]['delete']),
          ),
          'class' => array('draggable'),
        );
      }
    }
    // The table headers
    $header = array(
      '',
      t('Weight'),
      t('Title'),
      t('Path (main link and sub links)'),
      t('Icon'),
      t('Enabled'),
      t('Delete'),
    );

    $table = array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array(
        'id' => array('widget-menu-table'),
        'max-width' => '100%',
      ),
    );
    // Theme it as a table
    $output = theme('table', $table);

    // Render the form
    $output .= drupal_render_children($form);
  }
  else {
    $output = drupal_render_children($form);
  }

  // Return the themed activities table
  return $output;
}
