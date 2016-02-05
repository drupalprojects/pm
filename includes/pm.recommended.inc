<?php
/**
 * @file
 * Recommended modules helper functions.
 */

/**
 * Generates a table of recommended modules.
 */
function pm_recommended_modules_table() {
  // Initialise arrays.
  $header = array(
    'name' => t('Module'),
    'functionality' => t('Functionality'),
    'operations' => t('Operations'),
  );

  $rows = array();

  // Retrieve row information.
  $modules = module_invoke_all('pm_recommended_modules_info');
  
  foreach ($modules as $module) {
    $rows[] = array(
      'name' => $module['name'],
      'functionality' => $module['functionality'],
      'operations' => '',
    );
  }

  // Generate table.
  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('No additional modules are currently recommended.'),
  ));
}

/**
 * Implements hook_pm_recommended_modules_info().
 */
function pm_pm_recommended_modules_info() {
  return array(
    'fieldgroup' => array(
      'name' => t('Field Group'),
      'functionality' => t('Allows fields to be structured into labelled groups. This module is pre-configured by Project Management to work "out of the box", although the configuration can also be customised.'),
    ),
  )
}