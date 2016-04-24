<?php
/**
 * @file
 * Relationship handler for PM Parents.
 */

/**
 * Relationship handler for PM Parents.
 */
class pm_handler_relationship_parents extends views_handler_relationship {

  /**
   * Get this field's label.
   */
  public function option_definition() {
    $options = parent::option_definition();

    $options['node_type'] = array(
      'default' => NULL,
    );
    return $options;
  }

  /**
   * Define options form.
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $none = array('none' => '- No filter -');

    $form['node_type'] = array(
      '#type' => 'select',
      '#title' => t('Content type filter'),
      '#options' => array_merge($none, node_type_get_names()),
      '#default_value' => isset($this->options['node_type']) ? $this->options['node_type'] : 'none',
    );
  }

  public function query() {
    $this->ensure_my_table();

    // Add a join to pm_index.
    $index_join = new views_join();
    $index_join->definition = array(
      'left_table' => 'node',
      'left_field' => 'nid',
      'table' => 'pm_index',
      'field' => 'nid',
      'type' => !empty($this->options['required']) ? 'INNER' : 'LEFT',
    );
    $index_join->options = $this->options;
    $index_join->construct();

    $index_alias = $this->query->add_table('pm_index', NULL, $index_join);
    
    // Add a join from pm_index back to node.
    
    // Check whether this join needs to be restricted on content type.
    $node_join_extra = array();
    if ($this->options['node_type'] != 'none') {
      $node_join_extra[] = array(
        'field' => 'type',
        'value' => $this->options['node_type'],
      );
    }
    
    $node_join = new views_join();
    $node_join->definition = array(
      'left_table' => $index_alias,
      'left_field' => 'parent_nid',
      'table' => 'node',
      'field' => 'nid',
      'type' => 'INNER',
      'extra' => $node_join_extra,
    );
    $node_join->options = $this->options;
    $node_join->construct();
    
    $this->query->add_relationship($index_alias, $node_join, 'node', $index_alias);

    // Add access tags if the base table provide it.
    if (empty($this->query->options['disable_sql_rewrite']) && isset($table_data['table']['base']['access query tag'])) {
      $access_tag = $table_data['table']['base']['access query tag'];
      $this->query->add_tag($access_tag);
    }
  }
    
}
