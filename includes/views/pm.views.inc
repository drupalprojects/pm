<?php
/**
 * @file
 * Views integrations.
 */

/**
 * Implements hook_views_data().
 */
function pm_views_data() {
  $data = array();

  $data['pm_index']['table']['group'] = t('Drupal PM');
  $data['pm_index']['table']['base'] = array(
    'title' => t('PM Index'),
  );
  $data['pm_index']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  // The parent_nid field.
  $data['pm_index']['parent_nid'] = array(
    'title' => t('PM Organization'),
    'help' => t('PM Organization the content is attached to.'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'parent_nid',
      'handler' => 'views_handler_relationship',
      'label' => t('PM Parents'),
    ),
  );
  return $data;
}

/**
 * Implements hook_views_data_alter().
 */
function pm_views_data_alter(&$data) {
  $data['node']['pm_handler_area_addcontent'] = array(
    'title' => t('PM Add Content Link'),
    'help' => t('Provides a link to add new content of the specified type to users with permissions to do so.'),
    'area' => array(
      'handler' => 'pm_handler_area_addcontent',
      'group' => t('Drupal PM'),
    ),
  );

  $data['node']['pm_handler_field_operation'] = array(
    'title' => t('PM Operations'),
    'help' => t('Exposes operations that could be performed on the given node.'),
    'field' => array(
      'handler' => 'pm_handler_field_operation',
      'group' => t('Drupal PM'),
    ),
  );
}
