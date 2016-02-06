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
    '#collapsed' => FALSE,
    '#weight' => $w++,
  );

  $form['recommended']['help'] = array(
    '#type' => 'markup',
    '#markup' => t('The following modules significantly enhance Project Management and are recommended for installation.'),
  );

  module_load_include('inc', 'pm', 'includes/pm.recommended');

  $form['recommended']['table'] = array(
    '#type' => 'markup',
    '#markup' => pm_recommended_modules_table(),
  );

  $form['recommended']['pm_recommended_ignore'] = array(
    '#type' => 'checkbox',
    '#title' => t('Ignore missing recommended modules'),
    '#description' => t('When selected, no warnings will be shown if recommended modules are missing.'),
    '#default_value' => variable_get('pm_recommended_ignore', FALSE),
  );

  $form['icons'] = array(
    '#type' => 'fieldset',
    '#title' => t('Icons'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => $w++,
  );

  $icon_options = array(
    PM_ICON_SET_DEFAULT_BEHAVIOUR => t('<strong>Default:</strong> Displays fontawesome icons if the fontawesome module is installed. Otherwise reverts to PM legacy icons.'),
    PM_ICON_SET_FONTAWESOME => t('<strong>fontawesome icons:</strong> Displays fontawesome icons. Requires fontawesome module and libraries to be installed.'),
    PM_ICON_SET_STORM => t('<strong>PM legacy icons:</strong> Displays PM legacy icons. Deprecated - this option will be removed in a future release.'),
    PM_ICON_SET_NO_ICON => t('<strong>None:</strong> Hides all icons. Select this option if the icons available do not suit your selected theme.'),
  );

  $form['icons']['pm_icon'] = array(
    '#type' => 'radios',
    '#multiple' => FALSE,
    '#title' => t('Project Management icons'),
    '#options' => $icon_options,
    '#default_value' => variable_get('pm_icon', PM_ICON_SET_DEFAULT_BEHAVIOUR),
    '#weight' => $w++,
  );
  
  if (module_exists('fontawesome')) {
    $fontawesome_status = 'Module is enabled.';
  }
  else {
    $fontawesome_status = 'Module is not enabled.';
  }
  
  $form['icons']['fontawesome_status'] = array(
    '#type' => 'item',
    '#title' => t('fontawesome status'),
    '#markup' => $fontawesome_status,
  );

  return system_settings_form($form);
}
