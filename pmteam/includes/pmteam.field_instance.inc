<?php
/**
 * @file
 * pmteam.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function pmteam_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'node-pmteam-pmteam_user'
  $field_instances['node-pmteam-pmteam_user'] = array(
    'bundle' => 'pmteam',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Select the users who belongs to the team',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'entityreference',
        'settings' => array(
          'link' => FALSE,
        ),
        'type' => 'entityreference_label',
        'weight' => 1,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'pmteam_user',
    'label' => 'Members',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_select',
      'weight' => -18,
    ),
  );

  // Exported field_instance: 'node-pmteam-pmteam_organization'
  $field_instances['node-pmteam-pmteam_organization'] = array(
    'bundle' => 'pmteam',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Assigns all users from the selected organization to the team',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'entityreference',
        'settings' => array(
          'link' => FALSE,
        ),
        'type' => 'entityreference_label',
        'weight' => 2,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'pmteam_organization',
    'label' => 'Organization',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_select',
      'weight' => -17,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Assigns all users from the selected organization to the team');
  t('Members');
  t('Organization');
  t('Select the users who belongs to the team');

  return $field_instances;
}
