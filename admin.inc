<?php
// $Id: admin.inc,v 1.1 2010/11/10 08:40:51 zhangtaihao Exp $
/**
 * @file
 * Admin interface handler.
 */

/**
 * Overview form.
 */
function uuid_resolver_overview_form(&$form_state) {
  $resolvers_info = uuid_resolver_get_resolvers();

  $form['#header'] = array(
    array(
      'data' => t('Name'),
      'field' => 'title',
      'sort' => 'asc',
    ),
    array(
      'data' => t('Path'),
      'field' => 'base_path',
    ),
    t('Operations'),
    t('Enabled'),
  );

  // Build table
  foreach ($resolvers_info as $resolver => $info) {
    $enabled = uuid_resolver_is_enabled($resolver);

    $row = array(
      '#attributes' => array(
        'class' => $enabled ? 'enabled' : 'disabled'
      ),
    );
    $row['title'] = array(
      '#value' => check_plain($info['title']),
    );
    $row['path'] = array(
      '#value' => check_plain(uuid_resolver_get_base_path($resolver) . '/%'),
    );
    $row['operations'] = array(
      '#value' => filter_xss(l(t('Configure'), 'admin/settings/uuid_resolver/' . check_url($resolver), array('class' => 'uuid-resolver-configure'))),
    );
    $row[$resolver] = array(
      '#type' => 'checkbox',
      '#default_value' => $enabled,
      '#attributes' => array('class' => 'checkbox-enabled'),
    );

    $form[$resolver] = $row;
  }

  $form['actions'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="uuid-resolver-actions">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#attributes' => array('class' => 'uuid-resolver-submit'),
  );

  $form['suppress_message'] = array(
    '#type' => 'hidden',
    '#default_value' => FALSE,
  );

  return $form;
}

/**
 * Sort resolvers by field.
 * @param $resolvers_info  Array of resolver info keyed by resolver name.
 * @param $ts  An initialised tablesort context.
 */
function _uuid_resolver_overview_sort_resolvers(&$resolvers_info, $ts) {
  _uuid_resolver_overview_compare_resolvers(NULL, NULL, array($ts['sql'] => $ts['sort'] == 'asc'));
  uasort($resolvers_info, '_uuid_resolver_overview_compare_resolvers');
}

/**
 * Sort comparison callback.
 * @param $a  Resolver to be compared.
 * @param $b  Another resolver to be compared.
 * @param $criteria  If set, initialises the comparison critera.
 */
function _uuid_resolver_overview_compare_resolvers($a, $b, $criteria = NULL) {
  static $_criteria = array('name' => TRUE);
  if (isset($criteria)) {
    $_criteria = array();
    if (is_string($criteria) && !empty($criteria)) {
      $_criteria[$criteria] = TRUE;
    }
    elseif (is_array($criteria)) {
      $_criteria = $criteria;
    }
    if (!isset($_criteria['name'])) {
      $_criteria['name'] = TRUE;
    }
    return NULL;
  }

  // Sort
  $diff = 0;
  foreach ($_criteria as $order => $sort) {
    if (!array_key_exists($order, $a) || !array_key_exists($order, $b)) {
      continue;
    }
    $dir = $sort ? 1 : -1;
    // Test equal values
    if ($a[$order] === $b[$order]) {
      continue;
    }
    // Test NULL values
    if (!isset($a[$order])) {
      $diff = -$dir;
      break;
    }
    elseif (!isset($a[$order])) {
      $diff = $dir;
      break;
    }
    // Test ordinal values
    if ($a[$order] > $b[$order]) {
      $diff = $dir;
      break;
    }
    else {
      $diff = -$dir;
      break;
    }
  }
  return $diff;
}

/**
 * Renderer for uuid_resolver_overview_form().
 */
function theme_uuid_resolver_overview_form($form) {
  drupal_set_title(t('UUID resolvers'));
  drupal_add_js(drupal_get_path('module', 'uuid_resolver') . '/admin.js');
  drupal_add_css(drupal_get_path('module', 'uuid_resolver') . '/admin.css');
  $output = '<p>' . l(t('Refresh list'), 'admin/settings/uuid_resolver/refresh') . '</p>';

  // Set up sorting
  $resolvers_info = uuid_resolver_get_resolvers(TRUE);
  $ts = tablesort_init($form['#header']);
  _uuid_resolver_overview_sort_resolvers($resolvers_info, $ts);

  $rows = array();
  foreach ($resolvers_info as $resolver => $info) {
    if (isset($form[$resolver])) {
      $row = array();
      foreach (element_children($form[$resolver]) as $col) {
        $row[$col] = drupal_render($form[$resolver][$col]);
      }
      $rows[] = array(
        'data' => $row,
      ) + (array) $form[$resolver]['#attributes'];
    }
  }

  if (count($rows)) {
    $output .= theme('table', $form['#header'], $rows, array('id' => 'uuid-resolver-overview-table'));
  }

  $output .= drupal_render($form);
  return $output;
}

/**
 * Overview form submit handler.
 */
function uuid_resolver_overview_form_submit($form, $form_state) {
  $values = $form_state['values'];
  foreach (uuid_resolver_get_resolvers() as $resolver => $resolver_info) {
    uuid_resolver_set_enabled($resolver, $values[$resolver]);
  }
  if (!$values['suppress_message']) {
    drupal_set_message(t('Saved UUID resolver list.'));
  }
  uuid_resolver_cache_clear();
}

/**
 * Settings form.
 */
function uuid_resolver_settings_form() {
  drupal_add_js(drupal_get_path('module', 'uuid_resolver') . '/admin.js');

  $form['http_code'] = array(
    '#type' => 'fieldset',
    '#title' => t('HTTP response code'),
    '#description' => t('Specify the response code Drupal should provide when redirecting. See !rfc for other possible values.', array('!rfc' => l(t('RFC 2616'), 'http://www.ietf.org/rfc/rfc2616.txt'))),
    '#tree' => TRUE,
  );

  $http_code_map = _uuid_resolver_http_code_map();
  $http_codes = array_keys($http_code_map);
  $http_code = uuid_resolver_get_http_code();

  $form['http_code']['default'] = array(
    '#type' => 'radios',
    '#options' => _uuid_resolver_http_code_map() + array(-1 => t('Custom (specify below)')),
    '#default_value' => in_array($http_code, $http_codes, TRUE) ? $http_code : -1,
    '#attributes' => array('class' => 'uuid-resolver-http-code-default'),
    '#prefix' => '<div id="uuid-resolver-http-code-default-options">',
    '#suffix' => '</div>',
  );
  $form['http_code']['custom'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom response code'),
    '#description' => t('Warning: providing an invalid code results in undefined behaviour on redirect.'),
    '#default_value' => in_array($http_code, $http_codes, TRUE) ? '' : $http_code,
    '#size' => 3,
    '#maxlength' => 3,
    '#attributes' => array('class' => 'uuid-resolver-http-code-custom'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );

  return $form;
}

/**
 * Default Drupal/PHP HTTP response code-description map.
 * @return  An array of HTTP response code descriptions keyed by code.
 */
function _uuid_resolver_http_code_map() {
  return array(
    301 => t('301 Moved Permanently'),
    302 => t('@http-302 (default in Drupal/PHP)', array('@http-302' => t('302 Found'))),
    303 => t('303 See Other'),
    304 => t('304 Not Modified'),
    305 => t('305 Use Proxy'),
    307 => t('307 Temporary Redirect'),
  );
}

/**
 * Settings form validation handler.
 */
function uuid_resolver_settings_form_validate(&$form, &$form_state) {
  $values = $form_state['values'];
  if ($values['http_code']['default'] == -1) {
    if ($values['http_code']['custom'] == '') {
      form_error($form['http_code']['custom'], t('Please enter a custom HTTP response code.'));
    }
    elseif (!ctype_digit($values['http_code']['custom'])) {
      form_error($form['http_code']['custom'], t('Enter only numbers for the HTTP response code.'));
    }
    else {
      $custom = (int) $values['http_code']['custom'];
      if ($custom < 100 || $custom >= 600) {
        form_error($form['http_code']['custom'], t('Invalid HTTP response code.'));
      }
    }
  }
}

/**
 * Settings form submit handler.
 */
function uuid_resolver_settings_form_submit($form, $form_state) {
  $values = $form_state['values'];
  $http_code = $values['http_code']['default'] == -1 ?
      (int) $values['http_code']['custom'] :
      $values['http_code']['default'];
  variable_set('uuid_resolver_http_code', $http_code);
  drupal_set_message(t('The settings have been saved.'));
  $form_state['redirect'] = 'admin/settings/uuid_resolver';
}

/**
 * Implementation of hook_forms.
 */
function uuid_resolver_forms() {
  $forms = array();
  $resolvers = uuid_resolver_get_resolvers();
  foreach ($resolvers as $resolver => $resolver_info) {
    $forms['uuid_resolver_' . $resolver . '_settings_form'] = array(
      'callback' => 'uuid_resolver_resolver_settings_form',
      'callback arguments' => array($resolver_info),
    );
  }
  return $forms;
}

/**
 * Resolver settings page.
 */
function uuid_resolver_resolver_settings($resolver_info) {
  drupal_set_title(t('Configure settings for %resolver resolver.', array('%resolver' => t($resolver_info['title']))));
  drupal_add_js(drupal_get_path('module', 'uuid_resolver') . '/admin.js');
  return drupal_get_form('uuid_resolver_' . $resolver_info['name'] . '_settings_form', $resolver_info);
}

/**
 * Resolver settings form.
 */
function uuid_resolver_resolver_settings_form(&$form_state, $resolver_info) {
  $resolver = $resolver_info['name'];
  $form_state['resolver'] = $resolver;
  $form_state['resolver_info'] = $resolver_info;

  $form['#attributes']['class'] = 'uuid-resolver-resolver-settings-form';
  $form['#validate'][] = 'uuid_resolver_resolver_settings_form_validate';
  $form['#submit'][] = 'uuid_resolver_resolver_settings_form_submit';

  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable this resolver'),
    '#default_value' => uuid_resolver_is_enabled($resolver),
  );
  $form['base_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Base path'),
    '#field_suffix' => '/%',
    '#description' => t('Enter the base path from which this UUID resolver processes UUID requests. Warning: this value could override other menu callbacks and/or path aliases.'),
    '#default_value' => uuid_resolver_get_base_path($resolver),
    '#size' => 15,
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
    '#weight' => 10,
  );
  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('uuid_resolver_resolver_settings_form_cancel'),
    '#weight' => 11,
  );

  return $form;
}

/**
 * Resolver settings form validate callback.
 */
function uuid_resolver_resolver_settings_form_validate($form, &$form_state) {
  $values = $form_state['values'];

  // Clean up path
  $base_path = $values['base_path'];
  if (preg_match('`\s|^/|/$|/{2,}`', $base_path)) {
    form_set_error('base_path', t('Invalid patterns found in base path.'));
  }
}

/**
 * Resolver settings form submit callback.
 */
function uuid_resolver_resolver_settings_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $resolver = $form_state['resolver'];
  $resolver_info = $form_state['resolver_info'];
  uuid_resolver_set_enabled($resolver, $values['enabled']);
  uuid_resolver_set_base_path($resolver, $values['base_path']);
  uuid_resolver_cache_clear();

  drupal_set_message(t('Saved settings for @resolver resolver.', array('@resolver' => t($resolver_info['title']))));
  $form_state['redirect'] = 'admin/settings/uuid_resolver';
}

/**
 * Resolver settings form cancel callback.
 */
function uuid_resolver_resolver_settings_form_cancel() {
  drupal_goto('admin/settings/uuid_resolver');
}

/**
 * Refresh UUID resolver info.
 */
function uuid_resolver_refresh() {
  uuid_resolver_cache_clear();
  drupal_set_message(t('UUID resolver list has been refreshed.'));
  drupal_goto('admin/settings/uuid_resolver');
}
