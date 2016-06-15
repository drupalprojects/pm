<?php
/**
 * @file
 * Indexing functions.
 */

/**
 * Deletes index entries for a given child node.
 *
 * @param object $node
 *   The node object.
 */
function pm_delete_node_index_by_child($node) {
  if (variable_get('pm_maintain_index', TRUE)) {
    db_delete('pm_index')->condition('nid', $node->nid)->execute();        
  }
}

/**
 * Deletes index entries for a given parent node.
 *
 * @param object $node
 *   The node object.
 */
function pm_delete_node_index_by_parent($node) {
  if (variable_get('pm_maintain_index', TRUE)) {
    db_delete('pm_index')->condition('parent_nid', $node->nid)->execute();
  }
}

/**
 * Builds and inserts index entries for a given child node.
 *
 * The index lists all nodes that are parents of a given node entity.
 *
 * @param object $node
 *   The node object.
 */
function pm_build_node_index_by_child($node) {
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
    pm_write_node_index($node, $node);
  }
}

/**
 * Recursively insert index entries into the database.
 */
function pm_write_node_index($node, $child) {
  module_load_include('inc', 'pm', 'includes/pm.parent');
  $parent = pm_get_parent_node($child);

  if ($parent <> FALSE) {
    $query = db_insert('pm_index')->fields(array(
      'nid',
      'parent_nid',
      'parent_type',
    ));
    $query->values(array(
      'nid' => $node->nid,
      'parent_nid' => $parent->nid,
      'parent_type' => $parent->type,
    ));
    $query->execute();
    
    pm_write_node_index($node, $parent);
  }    
}

/**
 * Retrieve nids of all children of the given nid.
 */
function pm_get_children_from_index($parent_nid) {
  $result = db_select('pm_index', 'p')
    ->fields('p', array('nid'))
    ->condition('p.parent_nid', $parent_nid)
    ->execute();

  return $result;
}