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

  return system_settings_form($form);
}
