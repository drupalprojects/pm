<?php
/**
 * @file
 * Admin settings form functions.
 */

/**
 * Defines the administration settings form.
 */
function pm_admin_settings() {
  $form = array();
  $w = -10;

  $form['recommended'] = array(
    '#type' => 'fieldset',
    '#title' => t('Recommended modules'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => $w++,
  );

  $form['recommended']['help'] = array(
    '#type' => 'markup',
    '#markup' => t('The following modules significantly enhance Project Management and are recommended for installation.'),
  );
  
  $form['recommended']['pm_recommended_ignore'] = array(
    '#type' => 'checkbox',
    '#title' => t('Ignore missing recommended modules'),
    '#description' => t('When selected, no warnings will be shown if recommended modules are missing.'),
    '#default_value' => variable_get('pm_recommended_ignore', FALSE),
  );



  $form['recommended']['table'] = array(
    '#type' => 'markup',
    '#markup' => pm_get_recommended_modules_table(),
  );

  $form['icons'] = array(
    '#type' => 'fieldset',
    '#title' => t('Icons'),
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#weight' => $w++,
  );

  $icon_options = array(
    PM_ICON_SET_DEFAULT_BEHAVIOUR => t('<strong>Default Behaviour</strong> : Uses PM Legacy icons if fontawesome is not installed.'),
    PM_ICON_SET_FONTAWESOME => t('<strong>fontawesome</strong> : Requires fontawesome module and libraries to be installed.)'),
    PM_ICON_SET_STORM => t('<strong>PM legacy icons</strong> : Deprecated - this option will be removed in a future release)'),
    PM_ICON_SET_NO_ICON => t('<strong>No icons</strong> : The icons that ship with Project Management may not fit well with some themes. If this option is selected, icons will be hidden.'),
  );
  $form['icons']['pm_icon'] = array(
    '#type' => 'radios',
    '#multiple' => FALSE,
    '#title' => t('Project Management icons'),
    '#options' => $icon_options,
    '#default_value' => variable_get('pm_icon', PM_ICON_SET_DEFAULT_BEHAVIOUR),
    '#weight' => $w++,
  );

  // Dashboard settings.
  $form['pm_dashboard_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dashboard settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $types = module_invoke_all('pm_dashboard_types');
  foreach ($types as $type => $type_data) {
    $all_links_options = array();
    $all_links = pm_dashboard_get_links(FALSE, $type);
    foreach ($all_links as $link) {
      $all_links_options[$link['path']] = l($link['title'], $link['path']);
    }

    $default_dashboard_settings = variable_get('pm_' . $type . 'dashboard_settings', array());

    $form['pm_dashboard_settings'][$type] = array(
      '#type' => 'fieldset',
      '#title' => $type_data['title'],
      '#description' => $type_data['description'],
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['pm_dashboard_settings'][$type]['dashboard_links'] = array(
      '#infix' => $type,
    );

    $weight = 0;
    foreach ($all_links_options as $path => $title) {
      $form['pm_dashboard_settings'][$type]['dashboard_links'][$path][$type . '_pm_dashboard_link_active_' . $path] = array(
        '#type' => 'checkbox',
        '#title' => $title,
        '#default_value' => isset($default_dashboard_settings[$path]['active']) ? $default_dashboard_settings[$path]['active'] : TRUE,
      );
      $form['pm_dashboard_settings'][$type]['dashboard_links'][$path][$type . '_pm_dashboard_link_weight_' . $path] = array(
        '#type' => 'weight',
        '#default_value' => isset($default_dashboard_settings[$path]['weight']) ? $default_dashboard_settings[$path]['weight'] : $weight,
        '#delta' => 30,
      );
      $form['pm_dashboard_settings'][$type]['dashboard_links'][$path]['#value'] = $title;
      $weight++;
    }
  }

  if (empty($form['#submit'])) {
    $form['#submit'] = array();
  }
  $form['#submit'] = array('pm_admin_settings_form_submit');

  return system_settings_form($form);
}

/**
 * Submit function for admin settings form.
 */
function pm_admin_settings_form_submit($form, $form_state) {
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';
  // Reset.
  if ($op == t('Reset to defaults')) {
    $types = module_invoke_all('pm_dashboard_types');
    foreach ($types as $type => $type_data) {
      variable_del('pm_' . $type . 'dashboard_settings');
    };
    return;
  }

  // Get options.
  $types = module_invoke_all('pm_dashboard_types');
  foreach ($types as $type => $type_data) {
    $all_links = pm_dashboard_get_links(FALSE, $type);
    $settings = array();
    foreach ($all_links as $link) {
      $path = $link['path'];
      $settings[$path]['active'] = $form_state['values'][$type . '_pm_dashboard_link_active_' . $path];
      $settings[$path]['weight'] = $form_state['values'][$type . '_pm_dashboard_link_weight_' . $path];
    }
    variable_set('pm_' . $type . 'dashboard_settings', $settings);
  }
}
