<?php

class sm_generator_text extends ctools_export_ui {
  var $limit_generator = 30;

  function get_remaining_time () {
    $timer = timer_read('page');
    $max_time = ini_get('max_execution_time')*1000;
    return abs($max_time - $timer);
  }

  function  get_remaining_memory() {
    return parse_size(ini_get('memory_limit'))-memory_get_usage();
  }




  function token_options() {
    $entities['taxonomy_term'] = entity_get_info('taxonomy_term');
    $token_types = array();

    // Scan through the list of entities for supported token entities.
    foreach ($entities as $entity => $info) {
      $token_types[$entity] = !empty($info['token type']) ? $info['token type'] : $entity;
    }
    return $token_types;
  }

  function token_elements(&$form, $token_types) {
    $form['token_tree'] = array(
      '#theme' => 'token_tree',
      '#token_types' => array('site_by_brand', 'current-date', 'site'),
      '#global_types' => FALSE,
    );
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //
  //
  //
  //////////////////////////////////////////////////////////////////////////////////////
  public  function simpletext_generator($text, $array = FALSE) {
    drupal_set_time_limit(0);
    $textGeneratorOr = $this->textGeneratorOr($text);
    drupal_static_reset('textGeneratorOr');
    if(empty($textGeneratorOr)) {
      $textGeneratorOr= array($text);
    }
    //dpm($textGeneratorOr, '$textGeneratorOr');
    shuffle($textGeneratorOr);
    $new_text = array_shift($textGeneratorOr);
    $textGeneratorPermutations = $this->textGeneratorPermutations($new_text);
    drupal_static_reset('textGeneratorPermutations');
    shuffle($textGeneratorPermutations);
    if($array) {
      return $textGeneratorPermutations;
    }
    return array_shift($textGeneratorPermutations);
  }
  /**
   * http://zweroboy.net/script/funkciya-generatora-tekstov-na-php.html
   *
   * @param $text Это {скрипт|php функция} генерации текстов {на сайт|для {анкоров|сапы|sape}}
   * генерация или
   * @return array|int
   */
  public function textGeneratorOr($text) {
    $result = &drupal_static(__FUNCTION__);

    if(count($result) > $this->limit_generator ) {
      return array_values($result);
    }

//    if (($this->get_remaining_memory() < 128) || ($this->get_remaining_time() < 128)) {
//      return array_values($result);
//    }
    if ($count_pre_match= preg_match("/^(.*)\{([^\{\}]+)\}(.*)$/isU", $text, $matches)) {
      $p = explode('|', $matches[2]);
      reset($p);
      shuffle($p);
      foreach ($p as $comb) {
        $temporary_text = $matches[1] . $comb . $matches[3];
        $this->textGeneratorOr($temporary_text);
      }
    }
    else {
      $result[md5($text)] = $text;
      //пишем в базу
      return '';
    }
    //dpm(array_values(array_unique($result)));
    // Ограничеиваем размер массива генерации
    $return = array_values($result);
    return $return;
  }

  /**
   * Поддерживаются [+,+[+ и +переборы|перестановки]|вложенный синтаксис]
   * @param $text
   * @return array|int
   */
  public function textGeneratorPermutations($text) {

    $result = &drupal_static(__FUNCTION__);

//    // Ограничеваем размер массива генерации 300
    if(count($result) > $this->limit_generator ) {
      return array_values(array_unique($result));
    }

    if (preg_match("/^(.*)\[([^\[\]]+)\](.*)$/isU", $text, $matches)) {
      $glue =', ';
      $p = explode('|', $matches[2]);
      // TODO переменная $comb должна генериться на основе перестановок $this->Permutations
      shuffle($p);
      $comb = implode($glue, $p);
      $temporary_text = $matches[1] . $comb . $matches[3];
      $this->textGeneratorpermutations($temporary_text);
    }
    else {
      $result[md5($text)] = $text;
    }

    return array_values(array_unique($result));
  }

  function Permutations(&$srcarr, $cnt = 0, $arr = array(), $mask = array()){
    static $result_permutations;
    if (count($srcarr) == $cnt) {
      $result_permutations[] =  implode('', $arr);
      return;
    }
    for ($i = 0; $i < count($srcarr); $i++) {
      if (!$mask[$i]) {
        $mask[$i] = TRUE;
        $arr[$cnt] = $srcarr[$i];
        Permutations($srcarr, $cnt + 1, $arr, $mask);
        $mask[$i] = FALSE;
      }
    }
    return $result_permutations;
  }

//$srcarr = array('ru', 'en', 'ua');
//BackTrace($srcarr);

//Create node
//$dp = new dv_generator_plan();
//dpm($dp);
//$title = 'proba1';
//$body='proba-1 text';
//$deploy_name = 'deploy_http_test_1_rest';
  /**
   * @param $title
   * @param $deploy_name
   * @param string $pmenu_link
   * @param $type
   * @param int $plid
   * @param array $fields_array
   * @param int $weight
   * @return stdClass
   * $title, $deploy_name, $pmenu_link = '', $type, $plid = 0, $fields_array = array(), $weight = 0
   */
  function create_node($vars) {
    $title        = $vars['title'];
    $deploy_name  = $vars['deploy_name'];
    $pmenu_link   = empty($vars['pmenu_link']) ? '' : $vars['pmenu_link'];
    $type         = $vars['type'];
    $plid         = empty($vars['plid']) ? 0 : $vars['plid'];
    $fields_array = $vars['fields_array'];
    $weight       = empty($vars['weight']) ? 0 : $vars['weight'];
    $menu_title = empty($vars['menu_title'])? $title : $vars['menu_title'];

    // $form_id = $type . '_node_form';
    //$form = drupal_retrieve_form($form_id, $form_state);
    //drupal_prepare_form($form_id, $form, $form_state);
    global $user;
    $node = new stdClass();
    $node->name = $user->name;
    $node->uid = $user->uid;
    $node->language = LANGUAGE_NONE;
    $node->title = $title;
    //node type
    $node->type = $type;
    $node->created = time();
    $node->status = 1;
    $menu = array(
      'enabled'             => 1,
      'mlid'                => 0,
      'module'              => 'menu',
      'hidden'              => 0,
      'has_children'        => 0,
      'link_title'          => $menu_title,
      'customized'          => 0,
      'options'             => array(),
      'expanded'            => 0,
      'parent_depth_limit'  => 8,
      'description'         => $menu_title,
      'parent'              => $pmenu_link . ":{$plid}",
      'menu_name'           => $pmenu_link,
      'weight'              => $weight,
      'plid'                =>$plid,
    );

    $node->menu = $menu;

    foreach($fields_array as $key => $value) {
      $node->{$key} = $value;
    }
    $form_state['values']['created']  = time();
    $form_state['values']['language'] = LANGUAGE_NONE;
    $form_state['values']['title']    = $title;
    $form_state['values']['additional_settings__active_tab'] ='edit-menu';
    $form_state['values']['revision'] = 0;
    $form_state['values']['log']      = '';
    $form_state['values']['name']     = $user->name;
    $form_state['values']['status']   = 1 ;
    $form_state['values'] += $fields_array;
    $form_state['values']['menu'] = $menu ;
    $form_state['values']['op'] = t('Save');

    $form_state['node']               = $node;
    $form_state['build_info']['args'] = array($node);

    form_load_include($form_state, 'inc', 'node', 'node.pages');
    // drupal_form_submit($node->type . '_node_form', $form_state, (object)$node);
    $errors = form_get_errors();

    node_save($node);

    deploy_manager_add_to_plan($deploy_name, 'node', $node);

    //added menu link in plan
    if (!empty($node->menu['mlid'])) {
      deploy_manager_add_to_plan($deploy_name, 'menu_link', (object)$node->menu);
    }
    //    $files = $this-> file_extract_from_body($node->body['und'][0]['value']);
////    foreach($files as $fid => $file) {
//      deploy_manager_add_to_plan($deploy_name, 'file', $file);
//    }
    return $node;
  }

  /**
   * Ectract list of files fromt text and write this into databbase
   * @param $body
   * @return array files arrar(fid => object file )
   */
  public function file_extract_from_body($body) {
    $files = array();
    $scheme = file_default_scheme();
    $path_files_saved = variable_get('file_' . $scheme . '_path', conf_path() . '/files') . '/';
    $path_files_saved = str_replace('/', '\/', $path_files_saved);
    $pattern = '/'. $path_files_saved . '([^\s\/]+)\b/is';
    if(preg_match_all($pattern, $body, $matches)) {
      if(empty($matches[1])) {
        return ;
      }
    } else {
      return;
    }
    foreach($matches[1] as $val) {
      $uri = $scheme .'://' . $val;
      $files = file_load_multiple(array(), array('uri' => $uri));
      if(empty($files)) {
        $file = new StdClass();
        $file->uri = $uri;
        $file->filename = drupal_basename($uri);
        $file->filemime = file_get_mimetype($file->filename);
        $file->status   =  FILE_STATUS_PERMANENT;;
        $file = file_save($file);
        dpm($file, 'file saved');
        $files[$file->fid] = $file;
      }
      return $files;
    }
    //dpm( $matches[1], $pattern);
  }

  public  function test_file_extact() {
    $text = '<p><img alt="" src="http://dorvei/sites/default/files/hodovaya.jpg" style="height:225px; width:300px" /></p>

<p><img alt="" src="http://dorvei/sites/default/files/hodovaya_14.jpg" style="height:342px; width:194px" /></p>

<p><img alt="" src="http://dorvei/sites/default/files/imaecf7.tmp_0.png" style="height:516px; width:476px" /></p>
<span>Слава богу, сейчас кнопки и формы заказа де-факто стандартизированы. Ужасно было бы, если каждый сайт предлагал бы свои «интересные и яркие» способы положить товар в корзину.</span></p>
';
    $this->file_extract_from_body($text);
  }

  /**
   *$dp = new dv_generator_plan();
  $dp->test_node_create();
   */
  function test_node_create() {
    $title = 'proba' . rand(0, 100);
    $deploy_name = ' body ' . rand(0, 100);
    $pmenu_link = 'menu-menu-1';
    $type = 'model_brand_services';
    $plid = 0;
    $fields_array = array();
    $weight = 0;
    $vars = compact('title', 'deploy_name', 'pmenu_link', 'type', 'plid', 'fields_array', 'weight');
    $this->create_node($vars);

  }

  function services_by_brand($plan) {
    $services_by_brand = array();
    foreach ($plan->endpoints_by_marks as $endpoint => $brand_id) {
//      $services_by_brand[$endpoint][$brand_id]  =
    }
  }

  function test_connection() {
    $fp = fsockopen("217.197.236.77", 80, $errno, $errstr, 30);
    if (!$fp) {
      echo "$errstr ($errno)<br />\n";
    }
  }

}// end of class
