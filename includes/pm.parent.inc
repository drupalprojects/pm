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
      _pm_log_error($e);
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
    _pm_log_error($e);
  }
}

/**
 * Check if the parent field of a given node has changed value.
 */
function pm_parent_detect_change($node) {
  // Parent field can only have changed if a parent field exists.
  $wrapper = entity_metadata_wrapper('node', $node);
  $parent_field = variable_get("pm_permission_field_parent_reference_for_{$type}", NULL);
  if (empty($parent_field)) {
    return FALSE;
  }

  // Find new parent value.
  $parent_new = $wrapper->$parent_field->value();
  $new_id = (isset($parent_new->nid)) ? $parent_new->nid : NULL;

  // Find original parent value.
  $wrapper = entity_metadata_wrapper('node', $node->original);
  $parent_old = $wrapper->$parent_field->value();
  $old_id = (isset($parent_old->nid)) ? $parent_old->nid : NULL;

  // Detect and return whether the parent value has changed.
  return ($new_id != $old_id);
}

/**
 * Batch update children when a parent has changed.
 */
function pm_parent_update_children($node) {
  if (pm_parent_detect_change($node)) {
    // Find children to update.
    module_load_include('inc', 'pm', '/includes/pm.index');
    $chidren = pm_index_get_children($node->nid);

    // Set up a batch if there are children to update.
    if ($children) {
      $batch = array(
        'title' => t('Updating internal references for the @type "@title"', array('@type' => $node->type, '@title' => $node->title)),
        'operations' => array(
          array('pm_parent_update_children_batch_operation', array($children)),
        ),
        'finished' => 'pm_parent_update_children_batch_finished',
      );
      batch_set($batch);
    }
  }  
}

/**
 * Custom batch operation for updating children when a parent has changed.
 */
function pm_parent_update_children_batch_operation($nids, &$context) {
  if (empty($context['sandbox'])) {
    // Initiate multistep processing.
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = count($nids);
    $context['sandbox']['nids'] = drupal_map_assoc($nids);
    if (empty($nids)) {
      $context['finished'] = 1;
      return;
    }
  }

  $nids = array_slice($context['sandbox']['nids'], 0, 20);
  $nodes = node_load_multiple($nids, array(), TRUE);
  foreach ($nodes as $nid => $node) {
    // Only process an update if node loaded correctly.
    if (!empty($node)) {
      // Update index.
      pm_delete_node_index_by_child($node);
      pm_build_node_index_by_child($node);
      
      // Update permissions.
      node_access_acquire_grants($node);
    }
    unset($context['sandbox']['nids'][$nid]);
    $context['sandbox']['progress']++;
  }

  // Multistep processing: report progress.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Custom batch finish callback.
 */
function pm_parent_update_children_batch_finished($success, $results, $operations) {
  if (!$success) {
    drupal_set_message(t('An error occurred whilst updating internal references within Project Management.'), 'error');
  }
  cache_clear_all();
}
