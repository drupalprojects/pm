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

  // Generate table.
  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('No additional modules are currently recommended.'),
  ));
}
