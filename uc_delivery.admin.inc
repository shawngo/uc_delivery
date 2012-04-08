<?php

/**
 * @file Ubercart Delivery admin callback file.
 */

/**
 * Implements hook_admin_settings().
 */
function uc_delivery_admin_settings_form(&$form_state) {
  $form['general_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['general_settings']['uc_delivery_fs_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Pane / fieldset title'),
    '#default_value' => variable_get('uc_delivery_fs_title', t('Delivery fees')),
    '#description' => t('The title of the delivery fees fieldset.'),
  );
  $form['general_settings']['uc_delivery_fs_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Pane / fieldset description'),
    '#default_value' => variable_get('uc_delivery_fs_description', t('Select delivery options below.')),
    '#description' => t('The title of the delivery fees fieldset.'),
  );

  $form['list'] = array(
    '#type' => 'fieldset',
    '#title' => t('Delivery options'),
  );

  $result = db_query('SELECT * FROM {uc_delivery_options} ORDER BY oid ASC');
  $header = array(
    t('Option ID'),
    t('Enabled'),
    t('Weight'),
    t('Field type'),
    t('Fee title'),
    t('Fee description'),
    t('Options'),
    t('Operations'),
  );
  while($row = db_fetch_array($result)) {
    $fee_options = unserialize($row['fee_options']);
    $row['fee_options'] = '';
    foreach ($fee_options as $fo) {
      $row['fee_options'] .= $fo['amount'] . '|' . $fo['label'] . "\n";
    }
    $row['fee_options'] = '<pre>' . $row['fee_options'] . '</pre>';

    $row['enabled'] = $row['enabled'] ? 'Yes' : 'No';
    $row['weight'] = intval($row['weight']);
    $row['fee_element'] = check_plain($row['fee_element']);

    $row['operations'] = theme('item_list', array(
      l(t('Edit'), 'admin/store/settings/delivery/' . $row['oid'] . '/edit'),
        l(t('Delete'), 'admin/store/settings/delivery/' . $row['oid'] . '/delete'),

    ));
    $rows[] = $row;
  }

  $form['list']['items'] = array(
    '#type' => 'markup',
    '#value' => theme('table', $header, $rows),
  );

  $form['add_item'] = array(
    '#type' => 'markup',
    '#value' => l(t('Add option'), 'admin/store/settings/delivery/add'),
  );

  return system_settings_form($form);
}

/**
 * Callback for admin menu item.
 */
function uc_delivery_add_option_form(&$form_state) {
  $form = array();
  $form['fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add delivery fee'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['fs']['fee_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Fee title'),
    '#default_value' => '',
    '#required' => TRUE,
    '#description' => t('The title of the fee. This will be visible on the front-end.'),
  );
  $form['fs']['fee_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Fee description'),
    '#default_value' => '',
    '#rows' => 3,
    '#cols' => 60,
    '#required' => TRUE,
    '#description' => t('The description of the fee. This will be visible on the front-end.'),
  );
  $form['fs']['fee_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled'),
    '#default_value' => TRUE,
    '#required' => TRUE,
    '#description' => t('If not enabled this option will not be visible on the front-end.'),
  );
  $form['fs']['fee_options'] = array(
    '#type' => 'textarea',
    '#title' => t('Fee options'),
    '#default_value' => '',
    '#required' => TRUE,
    '#description' => t('Enter a pipe-delimited list of price|fee_title options.'),
  );
  $form['fs']['fee_weight'] = array(
    '#type' => 'select',
    '#title' => t('Fee weight'),
    '#default_value' => '',
    '#required' => TRUE,
    '#options' => array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
    '#description' => t('Select the display order for this fee.'),
  );
  $form['fs']['fee_element'] = array(
    '#type' => 'select',
    '#title' => t('Widget type'),
    '#default_value' => '',
    '#required' => TRUE,
    '#options' => array(
      'radios' => t('Radios'),
      'select' => t('Select menu'),
    ),
    '#description' => t('Select the type of form widget to use to select the fee.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add option'),
  );

  $form['submit_again'] = array(
    '#type' => 'submit',
    '#value' => t('Add another option'),
  );

  return $form;
}

function uc_delivery_add_option_form_submit($form, &$form_state) {
  $option = new stdClass();
  $option->title = $form_state['values']['fee_title'];
  $option->enabled = $form_state['values']['fee_enabled'];
  $option->weight = $form_state['values']['fee_weight'];
  $option->fee_element = $form_state['values']['fee_element'];
  $option->fee_description = $form_state['values']['fee_description'];

  $options = explode("\n", $form_state['values']['fee_options']);
  $fee_options = array();
  foreach ($options as $option_value) {
    $opt = explode('|', trim($option_value));
    $fee_options[] = array('amount' => number_format(doubleval($opt[0]), 2), 'label' => strip_tags($opt[1]));
  }

  $option->fee_options = serialize($fee_options);

  $res = drupal_write_record('uc_delivery_options', $option);
  if ($res) {
    drupal_set_message(t('New delivery fee has been created.'));
  }
  else {
    form_set_error('fee_title', t('An error has occurred in creating this option.'));
  }

  if ($form_state['values']['op'] == 'Add another option') {
    drupal_goto('admin/store/settings/delivery/add');
  }
  else {
    drupal_goto('admin/store/settings/delivery/overview');
  }
}

function uc_delivery_edit_option_form(&$form_state, $option) {
  $form = array();
  $form['fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Edit delivery fee'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['fs']['oid'] = array(
    '#type' => 'hidden',
    '#title' => t('Fee id'),
    '#value' => $option->oid,
  );
  $form['fs']['fee_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Fee title'),
    '#default_value' => $option->title,
    '#required' => TRUE,
    '#description' => t('The title of the fee. This will be visible on the front-end.'),
  );
  $form['fs']['fee_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Fee description'),
    '#default_value' => '',
    '#rows' => 3,
    '#cols' => 60,
    '#default_value' => $option->fee_description,
    '#required' => TRUE,
    '#description' => t('The description of the fee. This will be visible on the front-end.'),
  );
  $form['fs']['fee_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled'),
    '#default_value' => $option->enabled,
    '#required' => TRUE,
    '#description' => t('If not enabled this option will not be visible on the front-end.'),
  );

  $fee_options = unserialize($option->fee_options);
  foreach ($fee_options as $kopt) {
    if ($kopt['amount'] && $kopt['label']) {
      $fee_options_value .= check_plain($kopt['amount'] . '|' . $kopt['label']) . "\n";
    }
  }
  $form['fs']['fee_options'] = array(
    '#type' => 'textarea',
    '#title' => t('Fee options'),
    '#default_value' => trim($fee_options_value),
    '#required' => TRUE,
    '#description' => t('Enter a pipe-delimited list of price|fee_title options.'),
  );
  $form['fs']['fee_weight'] = array(
    '#type' => 'select',
    '#title' => t('Fee weight'),
    '#default_value' => $option->weight,
    '#required' => TRUE,
    '#options' => array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
    '#description' => t('Select the display order for this fee.'),
  );
  $form['fs']['fee_element'] = array(
    '#type' => 'select',
    '#title' => t('Widget type'),
    '#default_value' => $option->fee_element,
    '#required' => TRUE,
    '#options' => array(
      'radios' => t('Radios'),
      'select' => t('Select menu'),
    ),
    '#description' => t('Select the type of form widget to use to select the fee.'),
  );

  $form['update'] = array(
    '#type' => 'submit',
    '#value' => t('Update option'),
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete option'),
  );
  $form['back'] = array(
    '#type' => 'markup',
    '#value' => '<p>' . l(t('Return to list'), 'admin/store/settings/delivery/overview') . '</p>',
  );

  return $form;
}

function uc_delivery_edit_option_form_submit(&$form, &$form_state) {

  $option = new stdClass();
  $option->oid = $form_state['values']['oid'];
  $option->title = $form_state['values']['fee_title'];
  $option->enabled = $form_state['values']['fee_enabled'];
  $option->fee_description = $form_state['values']['fee_description'];
  $option->weight = $form_state['values']['fee_weight'];
  $option->fee_element = $form_state['values']['fee_element'];

  $options = explode("\n", $form_state['values']['fee_options']);
  $fee_options = array();
  foreach ($options as $option_value) {
    $opt = explode('|', trim($option_value));
    $fee_options[] = array('amount' => number_format(doubleval($opt[0]), 2), 'label' => strip_tags($opt[1]));
  }

  $option->fee_options = serialize($fee_options);

  $res = drupal_write_record('uc_delivery_options', $option, 'oid');

  if ($res) {
    drupal_set_message(t('Delivery fee has been updated.'));
  }
  else {
    form_set_error('fee_title', t('An error has occurred in updating this option.'));
  }

  drupal_goto(ltrim(request_uri(), '/'));
}
