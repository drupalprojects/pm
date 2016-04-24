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
    $def['left_table'] = 'node';
    $def['left_field'] = 'nid';

    $def['table'] = 'pm_index';
    $def['field'] = 'nid';

    if (!empty($this->options['required'])) {
      $def['type'] = 'INNER';
    }
    
    $join = new views_join();
    $join->definition = $def;
    $join->options = $this->options;
    $join->construct();
  }
    
}
