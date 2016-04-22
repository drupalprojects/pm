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
    $form['node_type'] = array(
      '#type' => 'select',
      '#title' => t('Content type'),
      '#options' => node_type_get_names(),
      '#default_value' => $this->options['node_type'],
    );
  }

}
