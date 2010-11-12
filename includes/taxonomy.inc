<?php
// $Id: taxonomy.inc,v 1.1 2010/11/10 08:40:51 zhangtaihao Exp $
/**
 * @file
 * Implements UUID resolver on behalf of taxonomy module.
 */

/**
 * Implements hook_uuid_resolver_info().
 */
function taxonomy_uuid_resolver_info() {
  $resolvers['taxonomy_term'] = array(
    'title' => 'Taxonomy term',
    'callback' => 'uuid_resolver_resolve_taxonomy_term',
  );
  return $resolvers;
}

/**
 * Resolve UUID for a term.
 * @param $uuid  UUID of term.
 * @return  Path or alias to term, or nothing if none matched.
 */
function uuid_resolver_resolve_taxonomy_term($uuid) {
  $tid = db_result(db_query("SELECT tid FROM {uuid_term_data} WHERE uuid = '%s'", $uuid));
  if ($tid) {
    $path = 'taxonomy/term/' . $tid;
    if (variable_get('uuid_resolver_taxonomy_term_use_alias', FALSE)) {
      return drupal_get_path_alias($path);
    }
    else {
      return $path;
    }
  }
  else {
    return FALSE;
  }
}

/**
 * Implements hook_form_FORM_ID_alter() for uuid_resolver_taxonomy_term_settings_form().
 */
function taxonomy_form_uuid_resolver_taxonomy_term_settings_form_alter(&$form, &$form_state) {
  $form['#submit'][] = 'uuid_resolver_taxonomy_term_extra_settings_form_submit';
  $form['use_alias'] = array(
    '#title' => t('Redirect to path alias.'),
    '#description' => t('If checked, the resolver will redirect to the aliased path instead of the system path.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('uuid_resolver_taxonomy_term_use_alias', FALSE),
  );
}

/**
 * Form submit handler.
 */
function uuid_resolver_taxonomy_term_extra_settings_form_submit($form, $form_state) {
  $values = $form_state['values'];
  variable_set('uuid_resolver_taxonomy_term_use_alias', (bool) $values['use_alias']);
}
