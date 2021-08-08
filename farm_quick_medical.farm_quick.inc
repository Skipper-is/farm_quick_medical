<?php

function farm_quick_medical_farm_quick_forms(){
  return array(
    'quick_medical'=>array(
        'label' =>t('Quick Medical'),
        'permission' => 'create farm_medical log entities',
        'form' => 'farm_quick_medical_form',
        'file'=> 'farm_quick_medical.farm_quick.inc',
    ),
  );
}

function farm_quick_medical_form($form, &$form_state){

  $form['quick_medical'] = array(
    '#type'=>'fieldset',
    '#title' => t('Record Medical logs'),
    '#description' => t('Use this form to record medical logs against animals'),
  );
  $form['quick_medical']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Log Name'),
    '#description' => t('The name of the log'),
    '#required' => TRUE,

  );

  $form['quick_medical']['timestamp'] = array(
  '#type' => 'date_select',
  '#title' => t('Date'),
  '#date_format' => 'M j Y H:i',
  '#date_type' => DATE_FORMAT_UNIX,
  '#date_year_range' => '-10:+3',
  '#default_value' => REQUEST_TIME,
  '#required' => TRUE,
  );

$form['quick_medical']['asset'] = array(
'#type'=>'textfield',
'#title' => t('Group/animal'),
'#description' => t('Select the group or animal the weight is for'),
'#autocomplete_path' => 'farm_asset/autocomplete/animal+group',

  );

  $form['quick_medical']['description'] = array(
    '#type' => 'text_format',
    '#title' => t("Notes"),
    '#format' => 'farm_format',
    '#required' => TRUE,

  );

  $form['quick_medical']['field_farm_vet'] = array(
    '#type'=>'textfield',
    '#title'=>t("Veterinarian"),
    '#description' => t('If a veterinarian was involved, enter their name here.'),
    '#autocomplete_path' => 'farm/fields/autocomplete/log/farm_medical/field_farm_vet'
  );

  $form['quick_medical']['medicines'] =  array(
      '#type' => 'fieldset',
      '#title' => t("Medicines"),
      '#prefix' => '<br>',
      '#collapsible' => TRUE,
      '#collapsed' =>  TRUE,
  );

  $form['quick_medical']['medicines']['medicine'] = array(
    '#type'=>'textfield',
    '#title' => t('Medicine Used'),
    '#description' => t("(Optional) The medicines administered. Create a new log for each medicine administered"),
    '#autocomplete_path' => 'farm/fields/autocomplete/field_collection_item/field_farm_quantity/field_farm_quantity_label',
  );

  $form['quick_medical']['medicines']['quantity'] = array(
  '#type' => 'textfield',
  '#title' => t('Quantity of medication'),
  '#element_validate' => array('element_validate_number'),
);
  $measureOptions = farm_quantity_measure_options();

  $form['quick_medical']['medicines']['measure'] = array(
    '#type' => 'select',
    '#title' => t('Type of Measurement'),
    '#options' => $measureOptions,
    '#default_value' => 'volume',
  );

  $form['quick_medical']['medicines']['units'] = array(
    '#type' => 'textfield',
    '#title' => t('Unit of measurement'),
    '#description' => t('Specify the units administered'),
    '#autocomplete_path' => 'taxonomy/autocomplete/field_farm_quantity_units',
  );


  $form['quick_medical']['medicines']['meat_days'] = array(
    '#type' => 'textfield',
    '#title' => t('Meat Withdrawal (Days)'),
    '#description' => t('The withdrawal (non-organic) period for meat, in days'),
    '#element_validate' => array('element_validate_number'),
  );

  $form['quick_medical']['medicines']['milk_days'] = array(
    '#type' => 'textfield',
    '#title' => t('Milk Withdrawal (Days)'),
    '#description' => t('The withdrawal (non-organic) period for milk, in days'),
    '#element_validate' => array('element_validate_number'),
  );

  $form['quick_medical']['medicines']['batch_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Batch Number of Medication'),

  );

  $form['quick_medical']['medicines']['expiry'] = array(
    '#type' => 'textfield',
    '#title' => t('Expiry Date of Medication'),

  );

  $flag_options = farm_flags_field_allowed_values();

  $form['quick_medical']['flags'] = array(
  '#type' => 'checkboxes',
  '#title' => t('Flags<br>'),
  '#options' => $flag_options,
  '#multiple' => TRUE,
  '#prefix' => '<div class="container-inline">',
  '#suffix' => '</div><br>'

);

  $form['quick_medical']['done'] = array(
    '#type'=> 'checkbox',
    '#title' => t('Done'),
    '#description' => t('Mark the log as completed'),
    '#default_value' =>TRUE,
  );

  $form['quick_medical']['submit'] = array(
	'#type' => 'submit',
	'#value' => t('Save Medical Record'),
  '#suffix' => '<br>',
	);

  return $form;
}

function farm_quick_medical_form_validate($form, &$form_state){
  $assets = array();
    $asset_names = drupal_explode_tags($form_state['values']['asset']);
    foreach($asset_names as $asset_name){
      $id = 0;
      $matches = array();
      $result =preg_match('/\\[id: ([0-9]+)\\]/', $asset_name, $matches);
      if(!empty($matches[$result])){
        $id = $matches[$result];
      }
      if(empty($id)){
        form_set_error('quick_medical] [asset',t('Could not load the asset. Make sure the asset ID is included. For example "Goat [id:123]"'));

      }
      $asset = farm_asset_load($id);
      if(!empty($asset)){
        $assets[$id] = $asset;
      }
      if(empty($asset)){
        form_set_error('quick_medical] [asset', t('Could not load the equipment. Make sure the asset name and ID are correct.'));
      }
    }
    $form_state['storage']['assets'] = $assets;


  if (!empty($form_state['values']['medicine'])){
    print_r("Not empty");
    print_r(empty($form_state['values']['quantity']));

    if(empty($form_state['values']['quantity'])){
      form_set_error('quick_medical] [medicines] [quantity', t('If medication is given, must enter quantity'));
    }
    if (empty($form_state['values']['units'])){
      form_set_error('quick_medical] [medicines] [units', t('If medication is given, must enter units'));
    }


  };
}


function farm_quick_medical_form_submit($form, &$form_state){
  $log_type = "farm_medical";
  $log_name = $form_state['values']['title'];
  $timestamp = strtotime($form_state['values']['timestamp']);
  $assets = $form_state['storage']['assets'];

  $notes = $form_state['values']['description']['value'];
  $vet = $form_state['values']['field_farm_vet'];
  $medicine = $form_state['values']['medicine'];
  $measure = $form_state['values']['measure'];
  $quantity = $form_state['values']['quantity'];
  $units = $form_state['values']['units'];
  $meat_days = $form_state['values']['meat_days'];
  $milk_days = $form_state['values']['milk_days'];
  $batch = $form_state['values']['batch_number'];
  $expiry = $form_state['values']['expiry'];
  $flags = array();
  if (!empty($form_state['values']['flags'])){
    foreach($form_state['values']['flags'] as $flag){
      if (strlen($flag) > 1 ){
        $flags[] = $flag;
      }
    }
  }
  $done = $form_state['values']['done'];
  $log = NULL;
  if (empty($medicine)){
    $log = farm_log_create('farm_medical', $log_name, $timestamp, $done, $assets, $notes ,$categories= array('Animals'));
  }
  else{
    $notes = $notes . t("Medication name: ") . $medicine . "\r\n";
    $medicineArray = array(
      array(
        'measure' => $measure,
        'value' => $quantity,
        'units' => $units,
        'label' => $medicine
      ),
    );
    if (!empty($batch)){
      $notes = $notes . t("Batch Number: ") . $batch . "\r\n";
    }

    if (!empty($expiry)){
      $notes = $notes . t("Expiry: ") . $expiry . "\r\n";
    }

    if (!empty($meat_days)){
      $meat_ok_date = farm_quick_medical_days_withdrawal($timestamp, FALSE, $meat_days);
      $meat_ok_date_organic = farm_quick_medical_days_withdrawal($timestamp, TRUE, $meat_days);
      $notes = $notes . t('Meat Withdrawal: ') . $meat_days . "d\r\n(Conventional OK date: " . date("d-m-Y",$meat_ok_date) . ', Organic OK date: ' . date("d-m-Y", $meat_ok_date_organic) . ")\r\n";
    }

    if(!empty($milk_days)){
      $milk_ok_date = farm_quick_medical_days_withdrawal($timestamp, FALSE, $milk_days);
      $milk_ok_date_organic = farm_quick_medical_days_withdrawal($timestamp, TRUE, $milk_days);
      $notes = $notes . t('Milk Withdrawal: ') . $milk_days . "d\r\n(Conventional OK date: " . date("d-m-Y",$milk_ok_date) . ', Organic OK date: ' . date("d-m-Y", $milk_ok_date_organic) . ")\r\n";
    }
    else{
        $notes = $notes . t('Milk Withdrawal: 0d');
    }


    $log = farm_quantity_log_create('farm_medical', $log_name, $timestamp, $done, $assets, $medicineArray, $notes, $categories = array('Animals'));
  }
    $entity_wrapper = entity_metadata_wrapper('log', $log);
    if (!empty($vet)){
    $entity_wrapper->field_farm_vet->set($vet);
    }
    if (!empty($flags)){
      foreach($flags as $flag){
      $entity_wrapper->field_farm_flags[] = $flag;
    }
  }
    $entity_wrapper->save();

    if(function_exists('farm_quick_entity_link')){
    		farm_quick_entity_link('farm_quick_medical_form', 'log', $log);
    	}
}

function farm_quick_medical_days_withdrawal($timestamp, $organic=TRUE, $days){
  # Takes a timestamp, and withdrawal days (can be decimal), and an organic flag. If not organic, just add the dates to the timestamp, otherwise, double the Withdrawal
  # period, and add that to the timestamp.

  $seconds_from_days = $days * 86400;
  if ($organic){
    $seconds_from_days *= 2;
  }
  return $timestamp+$seconds_from_days;
}
