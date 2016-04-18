<?php
/**
 * @file
 * Indexing functions.
 */

/**
 * Deletes index entries for a given node.
 *
 * @param object $node
 *   The node object.
 */
function pm_delete_node_index($node, $delete_by_parent = FALSE) {
  if (variable_get('pm_maintain_index', TRUE)) {
    db_delete('pm_index')->condition('nid', $node->nid)->execute();
        
    // Only remove parent_nid rows when explicitly specified.
    if ($delete_by_parent === TRUE) {
      db_delete('pm_index')->condition('parent_nid', $node->nid)->execute();
    }
  }
}

/**
 * Builds and inserts index entries for a given node.
 *
 * The index lists all nodes that are parents of a given node entity, and is
 * therefore maintained at the entity level.
 *
 * @param object $node
 *   The node object.
 */
function pm_build_node_index($node) {
  // We maintain a denormalized table of parent/node relationships, containing
  // only data for current, published nodes.
  $status = NULL;
  if (variable_get('pm_maintain_index', TRUE)) {
    // If a node property is not set in the node object when node_save() is
    // called, the old value from $node->original is used.
    if (!empty($node->original)) {
      $status = (int) (!empty($node->status) || (!isset($node->status) && !empty($node->original->status)));
    }
    else {
      $status = (int) (!empty($node->status));
    }
  }
  // We only maintain the index for published nodes.
  if ($status) {
    module_load_include('inc', 'pm', 'includes/pm.parent');
    // Hunt for PM Organization.
    // @todo Hunt for all parents.
    $parent_nid = pm_permission_get_parent_nid_of_node($node, 'pmorganization');;

    // Insert index entries for all the node's terms.
    if (!empty($parent_nid)) {
      $query = db_insert('pm_index')->fields(array(
        'nid',
        'parent_nid',
      ));
      $query->values(array(
        'nid' => $node->nid,
        'parent_nid' => $parent_nid,
      ));
      $query->execute();
    }
  }
}
