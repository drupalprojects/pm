<?php
/**
 * @file
 * pm.fields.inc
 */

/**
 * Create field bases if not present.
 * @param  [array] $field_bases [array ]
 * @return [type]               [description]
 */
function pm_field_bases_create_if_required($field_bases) {
  $result = array();
  if(!empty($field_bases) AND is_array($field_bases)){
    foreach ($field_bases as $key => $field_base) {
      $field_name = $field_base['field_name'];
      $result[$key] = pm_field_base_create_if_required($field_name, $field_base);
    }
  }
  return $result;
}

function pm_field_base_create_if_required($field_name , $field_base) {
  $field = field_info_field($field_name);
  if(!$field){
    $field = field_create_field($field_base);
  }
  return $field;
}


/**
 * Create field instances if not present.
 * @param  [array] $field_bases [array ]
 * @return [type]               [description]
 */
function pm_field_instances_create_if_required($field_instances) {
  $result = array();
  if(!empty($field_instances) AND is_array($field_instances)){
    foreach ($field_instances as $key => $field_instance) {
      $field_name = $field_instance['field_name'];
      $result[$key] = pm_field_instance_create_if_required($field_name, $field_instance);
    }
  }
  return $result;
}

function pm_field_instance_create_if_required($field_name , $field_instance) {
  $field = field_info_field($field_name);
  if(!$field){
    return FALSE;
  }
  return field_create_instance($field_instance);
}
