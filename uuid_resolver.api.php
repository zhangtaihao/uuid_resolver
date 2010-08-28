<?php
// $Id$

/**
 * @mainpage
 *
 * @verbinclude README.txt
 */

/**
 * @defgroup resolver_hooks UUID resolver hooks
 * @{
 * Hooks for modules to add UUID resolution functionality.
 */

/**
 * Register UUID resolvers. This hook adds resolvers for modules to generate
 * paths for a particular UUID path to redirect to.
 *
 * @return
 *   An array of information about UUID resolvers the module provides.
 *
 *   The array can have the following keys:
 *
 *     - 'title' : Required. Title of the resolver.
 *     - 'callback' : Required. Callback function for resolving UUID.
 *     - 'callback arguments' : An array of arguments to pass through to the
 *         callback function in addition to the request UUID.
 *
 * @see example_uuid_resolve_callback(), example2_uuid_resolve_callback(),
 *   node_uuid_resolver_info(), taxonomy_uuid_resolver_info(),
 *   user_uuid_resolver_info().
 */
function hook_uuid_resolver_info() {
  $resolvers['example'] = array(
    'title' => t('Example resolver'),
    'callback' => 'example_uuid_resolve_callback',
  );
  $resolvers['example2'] = array(
    'title' => t('Example resolver 2'),
    'callback' => 'example2_uuid_resolve_callback',
    'callback arguments' => array('foo', 'bar'),
  );
  return $resolvers;
}

/**
 * Example default resolver callback.
 *
 * @param $uuid
 *     Full valid 8-4-4-4-12 UUID string from the URL.
 *
 * @return  Path to redirect to.
 */
function example_uuid_resolve_callback($uuid) {
  // Look up object
  $node = node_get_by_uuid($uuid);
  if ($node->type == 'example') {
    return 'example/'.variable_get('example_setting', '0').'/'.$node->nid;
  }
}

/**
 * Example resolver callback with extra arguments.
 *
 * @param $uuid
 *     Full valid 8-4-4-4-12 UUID string from the URL.
 * @param $a1
 *     First argument from callback arguments.
 * @param $a2
 *     Second argument from callback arguments.
 *
 * @return
 *     Path to redirect to.
 */
function example2_uuid_resolve_callback($uuid, $a1, $a2) {
  if ($a1 == 'foo' && $a2 == 'bar') {
    $hash = md5($uuid);
    return 'foobar/'.$hash;
  }
}

/**
 * @}
 */

/**
 * @defgroup resolver_forms UUID resolver hooks
 * @{
 * Form configuration for resolvers.
 */

/**
 * Implementation of hook_form_FORM_ID_alter.
 *
 * Adds new settings to the resolver configurable via the settings form for the
 * UUID resolver by extending the form. By default, UUID resolver provides a
 * form for enabling/disabling a resolver and setting the base path for it to
 * register on. The form ID is formatted as:
 *
 *   uuid_resolver_RESOLVER_settings_form
 *
 * where RESOLVER is machine name of the UUID resolver.
 */
function hook_form_uuid_resolver_example_settings_form_alter(&$form, &$form_state) {
  $form['#validate'][] = 'example_resolver_form_validate';
  $form['#submit'][] = 'example_resolver_form_submit';
  $form['example'] = array(
    '#type' => 'textfield',
    '#title' => t('Example setting'),
    '#description' => t('This is an example setting.'),
    '#default_value' => variable_get('example_setting', '0'),
    '#size' => 10,
    '#maxlength' => 10,
    '#required' => TRUE,
  );
}

/**
 * Example form validation handler.
 */
function example_resolver_form_validate(&$form, &$form_state) {
  $example = $form_state['values']['example'];
  if (!empty($example) && !ctype_digit($example)) {
    form_set_error('example', t('Please enter only digits.'));
  }
}

/**
 * Example form submit handler.
 */
function example_resolver_form_submit($form, $form_state) {
  variable_set('example_setting', $form_state['values']['example']);
}

/**
 * @}
 */