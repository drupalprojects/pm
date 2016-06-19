<?php
/**
 * @file
 * Field group definitions.
 */

 /**
 * Implements hook_field_group_info().
 */
function pmteam_field_group_info() {
  $export = array();

  // References
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_references|node|pmteam|default';
  $field_group->group_name = 'group_references';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmteam';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'References',
    'weight' => '0',
    'children' => array(
      0 => 'pmteam_organization',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-references field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_references|node|pmteam|form';
  $field_group->group_name = 'group_references';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmteam';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'References',
    'weight' => '0',
    'children' => array(
      0 => 'pmteam_organization',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-references field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  // People
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_people|node|pmteam|default';
  $field_group->group_name = 'group_people';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmteam';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'People',
    'weight' => '0',
    'children' => array(
      0 => 'pmteam_user',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-people field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_people|node|pmteam|form';
  $field_group->group_name = 'group_people';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmteam';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'People',
    'weight' => '0',
    'children' => array(
      0 => 'pmteam_user',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-people field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  // Miscellaneous
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_miscellaneous|node|pmteam|default';
  $field_group->group_name = 'group_miscellaneous';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmteam';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Miscellaneous',
    'weight' => '1',
    'children' => array(
      0 => 'body',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-miscellaneous field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_miscellaneous|node|pmteam|form';
  $field_group->group_name = 'group_miscellaneous';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmteam';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Miscellaneous',
    'weight' => '1',
    'children' => array(
      0 => 'body',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-miscellaneous field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  return $export;
}
