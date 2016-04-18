<?php
/**
 * @file
 * Helper functions for working with PM parent fields.
 */

/**
 * Recursively hunts and find out the first parent node of given type.
 *
 * @param object $node
 *   The node for which the parent to be found.
 * @param string $parent_type
 *   The content type of parent node.
 * @param int $max_depth
 *   How many levels up should the function traverse.
 *
 * @return mixed
 *   nid of the parent node if found, FALSE otherwise.
 */
function pm_get_parent_nid_by_type($node, $parent_type, $max_depth = NULL) {

  if ($max_depth == NULL) {
    $max_depth = variable_get('pm_permission_relation_search_depth', 10);
  }

  if ($node->type == $parent_type) {
    return $node->nid;
  }
  if ($max_depth) {
    --$max_depth;
    try {
      $n = pm_get_parent_node($node);
      if (empty($n)) {
        return FALSE;
      }
      elseif ($n->type == $parent_type) {
        return $n->nid;
      }
      else {
        return pm_get_parent_nid_by_type($n, $parent_type, $max_depth);
      }
    }
    catch (Exception $e) {
      _pm_permission_watchdog_log($e);
    }
  }
  else {
    return FALSE;
  }
}

/**
 * Recursively hunts and find out the first parent node.
 *
 * @param object $node
 *   The node for which the parent to be found.
 *
 * @return mixed
 *   nid of the parent node if found, FALSE otherwise.
 */
function pm_get_parent_node($node) {
  $bundle_name = $node->type;
  $field_name = _pm_permission_get_field_name($bundle_name, 'parent');
  if (empty($field_name)) {
    return FALSE;
  }
  try {
    $wrapper = entity_metadata_wrapper('node', $node);
    $n = $wrapper->$field_name->value();
    if (empty($n)) {
      return FALSE;
    }
    else {
      return $n;
    }
  }
  catch (Exception $e) {
    _pm_permission_watchdog_log($e);
  }
}
