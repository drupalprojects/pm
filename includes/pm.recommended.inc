<?php
/**
 * @file
 * Recommended modules helper functions.
 */

function pm_recommended_modules_table() {

  $header = array(
    'name' => t('Module'),
    'functionality' => t('Functionality'),
    'operations' => t('Operations'),
  );

  $rows = array();

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('No additional modules are currently recommended.'),
  ));
}