<?php

/**
 * Admin form callback function().
 */
function video_playlist_admin_form() {

	$form = array();
	$form['configuration'] = array(
		'#type' => 'fieldset',
		'#title' => t('Video Playlist configuration')
	);

	$form['configuration']['para_field_mn'] = array(
		'#type' => 'textfield',
		'#title' => 'Paragraph machine name',
		'#default_value' => 'field_paragraph',
		'#description' => 'Sets the machine name for the paragraph field.'
	);

	$form['configuration']['full_setup_message'] = array(
		'#type' => 'markup',
		'#markup' => '<h3>Full Setup</h3><p>Click the button below to create the <em>Video Playlist</em> content type, <em>Playlist Video</em> paragraph, all required <em>fields</em>, <em>templates</em>, and <em>Test content</em>.</p>'
	);

	$form['configuration']['video_playlist_full_setup'] = array(
		'#type' => 'button',
		'#value' => t('Setup Full Video Playlist.'),
		'#ajax' => array(
			'callback' => 'video_playlist_full_setup',
		),
	);

	$form['configuration']['custom_setup_message'] = array(
		'#type' => 'markup',
		'#markup' => '<h3>Custom Setup</h3><p>Click the button below to create the <em>Playlist Video</em> paragraph, all required <em>fields</em>, <em>templates</em>, <em>Test content</em> and add it to the selected content type.</p>'
	);

	$options = node_type_get_types();
	$contentTypes = array();
	foreach($options as $option) {
		$contentTypes[$option->type] = $option->name;
	};

	$form['configuration']['video_playlist_custom_content_type'] = array(
		'#title' => t('Custom Content Type.'),
		'#type' => 'select',
		'#options' => $contentTypes,
		'#ajax' => array(
		    'callback' => 'video_playlist_get_fields_list',
		    'wrapper' => 'edit-video-playlist-custom-paragraph-field',
		    'event' => 'change',
		    'method' => 'replace',
		    'effect' => 'fade',
		),
		'#description' => t('Choose the content type to add the playlist to.'),
	);

	/*$instances = field_info_instances('node', reset($contentTypes));
	$options = array();
	foreach($instances as $key => $instance) {
		$options[$key] = $instance['label'];
	};

	$form['configuration']['video_playlist_custom_paragraph_field'] = array(
		'#title' => 'Custom Paragraph Field',
		'#type' => 'select',
		'#options' => $options,
		'#description' => t('Choose the paragraph field to add the playlist to.'),
		'#validated' => TRUE
	);*/

	$form['configuration']['video_playlist_custom_setup'] = array(
		'#type' => 'button',
		'#value' => t('Setup Custom Video Playlist.'),
		'#ajax' => array(
			'callback' => 'video_playlist_custom_setup',
		),
	);

	return $form;
}

/**
 * Add horizontal boolean to content type.
 */
function video_playlist_create_node_fields($contentType, $paraField) {

	// Check if our field is not already created.
	if (!field_info_field($paraField) && !in_array($contentType, field_info_field($paraField)['bundles']['node'])) {
		$field = array(
			'field_name' => $paraField,
			'type' => 'paragraphs',
			'cardinality' => -1,
		);
		field_create_field($field);
	};

	if(!field_info_instance('node', $paraField, $contentType)) {
		// Create the instance on the bundle.
		$instance = array(
			'field_name' => $paraField,
			'entity_type' => 'node',
			'bundle' => $contentType, // If your entity does not have bundles, this is the same as the entity type.
			'label' => 'Playlist Video',
			'required' => TRUE, // Field must have a value.
			'settings' => array(
				'allowed_bundles' => array('pvw' => 'pvw'),
			),
			'widget' => array(
				'type' => 'paragraphs_embed',
			),
    	);
    	field_create_instance($instance);
  	} else {
		$instance_info = field_info_instance('node', $paraField, $contentType);
		if(!isset($instance_info['settings']['allowed_bundles']['pvw'])) {
			$instance_info['settings']['allowed_bundles']['pvw'] = 'pvw';
			field_update_instance($instance_info);
		}else if($instance_info['settings']['allowed_bundles']['pvw'] == -1) {
			$instance_info['settings']['allowed_bundles']['pvw'] = 'pvw';
			field_update_instance($instance_info);
		}
  	}
}

/**
 * Create content type callback function().
 */
function video_playlist_create_content_type($paraField) {

	$message = 'Video Playlist content type already exists.';
	$contentType = node_type_load('video_playlist');

	if(!$contentType && empty($contentType)) {
		$message = 'Video content type created.';
		$contentType = array(
			'type'          => 'video_playlist',
			'name'          => 'Video Playlist',
			'description'   => 'Create a new video playlist.',
			'title_label'   => 'Video Playlist',
			'base'          => 'node_content',
			'custom'        => TRUE,
		);
 
		$node_type = node_type_set_defaults($contentType);
		node_type_save($node_type);
	}

	video_playlist_create_node_fields('video_playlist', $paraField);

	drupal_set_message($message);
}

/**
 * Create paragraph bundle callback function().
 */
function video_playlist_create_paragraph_bundles() {

	$message = 'Playlist Video paragraph bundle already exists.';
	$bundles = array_keys(field_info_bundles('paragraphs_item'));

	if(!in_array('pvw', $bundles)) {
		// Create new Bundle.
		$bundle = new stdClass();
		$bundle->name = 'Playlist Video Wrapper'; // user friendly label
		$bundle->bundle = 'pvw'; // machine name
		$bundle->locked = 0;
		$status = paragraphs_bundle_save($bundle);
	}

	if(!in_array('playlist_video', $bundles)) {
		$message = "Playlist Video paragraph bundle created.";
		// Create new Bundle.
		$bundle = new stdClass();
		$bundle->name = 'Playlist Video'; // user friendly label
		$bundle->bundle = 'playlist_video'; // machine name
		$bundle->locked = 0;
		$status = paragraphs_bundle_save($bundle);
	}

	// Check if our field is not already created.
	if (!field_info_field('field_playlist_is_horizontal')) {
		$field = array(
			'field_name' => 'field_playlist_is_horizontal',
			'type' => 'list_boolean',
            'label' => 'Check for a horizontal playlist.',
			'settings' => array(
				'allowed_values' => array(0 => 0, 1 => 1), //add the default values for the checkbox
			),
		);
		field_create_field($field);
	};

	if (!field_info_field('field_vpp')) {
		$field = array(
			'field_name' => 'field_vpp',
			'type' => 'paragraphs',
			'cardinality' => -1,
		);
		field_create_field($field);
	};

	// Check if our field is not already created.
	if (!field_info_field('field_playlist_video')) {
		$field = array(
			'field_name' => 'field_playlist_video',
			'type' => 'file',
		);
		field_create_field($field);
	}

	// Check if our field is not already created.
	if (!field_info_field('field_playlist_video_poster')) {
		$field = array(
			'field_name' => 'field_playlist_video_poster',
			'type' => 'image',
		);
		field_create_field($field);
	}

	// Check if our field is not already created.
	if (!field_info_field('field_playlist_video_title')) {
		$field = array(
			'field_name' => 'field_playlist_video_title',
			'type' => 'text',
		);
		field_create_field($field);
	}

	// Check if our field is not already created.
	if (!field_info_field('field_playlist_video_download')) {
		$field = array(
			'field_name' => 'field_playlist_video_download',
			'type' => 'list_boolean',
            'label' => 'Check to enable video download',
			'settings' => array(
				'allowed_values' => array(0 => 0, 1 => 1), //add the default values for the checkbox
			),
		);
		field_create_field($field);
	}

	// Check if our field is not already created.
	if (!field_info_field('field_playlist_video_description')) {
		$field = array(
			'field_name' => 'field_playlist_video_description',
			'type' => 'text_long',
		);
		field_create_field($field);
	}

	if(!field_info_instance('paragraphs_item', 'field_playlist_is_horizontal', 'pvw')) {
		// Create the instance on the bundle.
		$instance = array(
			'field_name' => 'field_playlist_is_horizontal',
			'entity_type' => 'paragraphs_item',
			'bundle' => 'pvw', // If your entity does not have bundles, this is the same as the entity type.
			'label' => 'Is Horizontal',
			'required' => FALSE, // Field must have a value.
			'widget' => array(
				'type' => 'options_onoff',
				'settings' => array(
					'display_label' => 1,
				),
			),
    	);
    	field_create_instance($instance);
  	};

	if(!field_info_instance('paragraphs_item', 'field_vpp', 'pvw')) {
		// Create the instance on the bundle.
		$instance = array(
			'field_name' => 'field_vpp',
			'entity_type' => 'paragraphs_item',
			'bundle' => 'pvw', // If your entity does not have bundles, this is the same as the entity type.
			'label' => 'Playlist Video',
			'required' => TRUE, // Field must have a value.
			'settings' => array(
				'allowed_bundles' => array('playlist_video' => 'playlist_video'),
			),
			'widget' => array(
				'type' => 'paragraphs_embed',
			),
    	);
    	field_create_instance($instance);
  	};

	if(!field_info_instance('paragraphs_item', 'field_playlist_video', 'playlist_video')) {
		// Create the instance on the bundle.
		$instance = array(
			'field_name' => 'field_playlist_video',
			'entity_type' => 'paragraphs_item',
			'bundle' => 'playlist_video', // If your entity does not have bundles, this is the same as the entity type.
			'label' => 'Playlist Video File',
			'required' => TRUE, // Field must have a value.
			'widget' => array(
				'type' => 'file_generic',
			),
            'settings' => array(
                'file_extensions' => 'mp4',
                'file_directory' => 'playlist/videos/mp4',
                'max_filesize' => '50MB',
            )
    	);
    	field_create_instance($instance);
  	}

	if(!field_info_instance('paragraphs_item', 'field_playlist_video_poster', 'playlist_video')) {
		// Create the instance on the bundle.
		$instance = array(
			'field_name' => 'field_playlist_video_poster',
			'entity_type' => 'paragraphs_item',
			'bundle' => 'playlist_video', // If your entity does not have bundles, this is the same as the entity type.
			'label' => 'Playlist Video Poster',
			'required' => TRUE, // Field must have a value.
			'widget' => array(
				'type' => 'image:image',
			),
            'settings' => array(
                'file_extensions' => 'png jpg jpeg',
                'file_directory' => 'playlist/videos/mp4/cover',
                'max_filesize' => '10MB'
            )
    	);
    	field_create_instance($instance);
  	}

	if(!field_info_instance('paragraphs_item', 'field_playlist_video_title', 'playlist_video')) {
		// Create the instance on the bundle.
		$instance = array(
			'field_name' => 'field_playlist_video_title',
			'entity_type' => 'paragraphs_item',
			'bundle' => 'playlist_video', // If your entity does not have bundles, this is the same as the entity type.
			'label' => 'Playlist Video Title',
			'required' => TRUE, // Field must have a value.
			'widget' => array(
				'type' => 'textfield',
			),
    	);
    	field_create_instance($instance);
  	}

	if(!field_info_instance('paragraphs_item', 'field_playlist_video_download', 'playlist_video')) {
		// Create the instance on the bundle.
		$instance = array(
			'field_name' => 'field_playlist_video_download',
			'entity_type' => 'paragraphs_item',
			'bundle' => 'playlist_video', // If your entity does not have bundles, this is the same as the entity type.
			'label' => 'Playlist Video Download',
			'required' => FALSE, // Field must have a value.
			'widget' => array(
				'type' => 'options_onoff',
				'settings' => array(
					'display_label' => 1,
				),
			),
    	);
    	field_create_instance($instance);
  	}

	if(!field_info_instance('paragraphs_item', 'field_playlist_video_description', 'playlist_video')) {
		// Create the instance on the bundle.
		$instance = array(
			'field_name' => 'field_playlist_video_description',
			'entity_type' => 'paragraphs_item',
			'bundle' => 'playlist_video', // If your entity does not have bundles, this is the same as the entity type.
			'label' => 'Playlist Video Description',
			'required' => TRUE, // Field must have a value.
			'widget' => array(
				'type' => 'text_textarea',
			),
    	);
    	field_create_instance($instance);
  	}

	drupal_set_message($message);
}

/**
 * Function to check if test content exists.
 */
function checkNodeExistsByTitle($type, $title) {
    $query = new EntityFieldQuery();
    $entities = $query->entityCondition('entity_type', 'node')
      ->propertyCondition('type', $type)
      ->propertyCondition('title', $title)
      ->range(0, 1)
      ->execute();
    return $entities;
  }

/**
 * Create test content callback function().
 */
function video_playlist_create_test_content($contentType, $paraField) {
	Global $user;

	$exists = checkNodeExistsByTitle($contentType, 'Video Playlist Test Content');
	$message = 'Test content node already exists.';

	if(!$exists && empty($exists)) {
		$message = 'Test content node created.';
		$node = new stdClass();
		$node->title = "Video Playlist Test Content";
		$node->type = $contentType;
		$node->language = LANGUAGE_NONE; 
		node_object_prepare($node); 
		$node->uid = $user->uid; 
		$node->status = 1;
		$node = node_submit($node);
		$newNode = $node;
	} else {
		$node = $exists['node'];
		$nid = reset($node)->nid;
		$newNode = node_load($nid);
	}

	$paraID = ($newNode->{$paraField}['und'][0]['value']) ? $newNode->{$paraField}['und'][0]['value'] : NULL;
	$message = 'Test paragraph content already exist.';

	if(!$paraID && empty($paraID)) {
		$message = 'Test paragraph content created.';
		$paraWrap = new ParagraphsItemEntity(array('field_name' => $paraField, 'bundle' => 'pvw'));
		$paraWrap->is_new = TRUE;
		$paraWrap->setHostEntity('node', $newNode);

		$paragraph = new ParagraphsItemEntity(array('field_name' => 'field_vpp', 'bundle' => 'playlist_video'));
		$paragraph->is_new = TRUE;
		$paragraph->setHostEntity('paragraphs_item', $paraWrap);

		$filename = 'video.mp4';
		$source = drupal_get_path('module', 'video_playlist') . '/test_content/' . $filename;
		$media = file_get_contents($source);
		$dest = 'public://' . $filename; // Or whatever
		$file = file_save_data($media, $dest, FILE_EXISTS_RENAME);

		if ($file) {
			// file_save_upload marks the file as temporary, we need to mark as permanent or it will be cleared from the database in a few hours
			$file->status = FILE_STATUS_PERMANENT;
			file_save($file);
		}

		$paragraph->field_playlist_video[LANGUAGE_NONE][0] = array(
			'fid' => $file->fid,
			'display' => 1,
			'description' => '',
		);

		$filename = 'poster.jpg';
		$source = drupal_get_path('module', 'video_playlist') . '/test_content/' . $filename;
		$media = file_get_contents($source);
		$dest = 'public://' . $filename;
		$file = file_save_data($media, $dest, FILE_EXISTS_RENAME);

		if ($file) {
			// file_save_upload marks the file as temporary, we need to mark as permanent or it will be cleared from the database in a few hours
			$file->status = FILE_STATUS_PERMANENT;
			file_save($file);
		}

		$paragraph->field_playlist_video_poster[LANGUAGE_NONE][0] = array(
			'fid' => $file->fid,
			'display' => 1,
			'description' => '',
		);

		$paragraph->field_playlist_video_title[LANGUAGE_NONE][0]['value'] = "Tel Aviv Beach";

		$paragraph->field_playlist_video_description[LANGUAGE_NONE][0]['value'] = "A video of Tel Aviv Beach.";

		$paragraph->save();

		$paragraph = new ParagraphsItemEntity(array('field_name' => 'field_vpp', 'bundle' => 'playlist_video'));
		$paragraph->is_new = TRUE;
		$paragraph->setHostEntity('paragraphs_item', $paraWrap);

		$filename = 'video01.mp4';
		$source = drupal_get_path('module', 'video_playlist') . '/test_content/' . $filename;
		$media = file_get_contents($source);
		$dest = 'public://' . $filename; // Or whatever
		$file = file_save_data($media, $dest, FILE_EXISTS_RENAME);

		if ($file) {
			// file_save_upload marks the file as temporary, we need to mark as permanent or it will be cleared from the database in a few hours
			$file->status = FILE_STATUS_PERMANENT;
			file_save($file);
		}

		$paragraph->field_playlist_video[LANGUAGE_NONE][0] = array(
			'fid' => $file->fid,
			'display' => 1,
			'description' => '',
		);

		$filename = 'poster01.jpg';
		$source = drupal_get_path('module', 'video_playlist') . '/test_content/' . $filename;
		$media = file_get_contents($source);
		$dest = 'public://' . $filename;
		$file = file_save_data($media, $dest, FILE_EXISTS_RENAME);

		if ($file) {
			// file_save_upload marks the file as temporary, we need to mark as permanent or it will be cleared from the database in a few hours
			$file->status = FILE_STATUS_PERMANENT;
			file_save($file);
		}

		$paragraph->field_playlist_video_poster[LANGUAGE_NONE][0] = array(
			'fid' => $file->fid,
			'display' => 1,
			'description' => '',
		);

		$paragraph->field_playlist_video_title[LANGUAGE_NONE][0]['value'] = "Scrolling iPhone XR";

		$paragraph->field_playlist_video_description[LANGUAGE_NONE][0]['value'] = "A video of Scrolling iPhone XR.";

		$paragraph->save();

		$paragraph = new ParagraphsItemEntity(array('field_name' => 'field_vpp', 'bundle' => 'playlist_video'));
		$paragraph->is_new = TRUE;
		$paragraph->setHostEntity('paragraphs_item', $paraWrap);

		$filename = 'video02.mp4';
		$source = drupal_get_path('module', 'video_playlist') . '/test_content/' . $filename;
		$media = file_get_contents($source);
		$dest = 'public://' . $filename; // Or whatever
		$file = file_save_data($media, $dest, FILE_EXISTS_RENAME);

		if ($file) {
			// file_save_upload marks the file as temporary, we need to mark as permanent or it will be cleared from the database in a few hours
			$file->status = FILE_STATUS_PERMANENT;
			file_save($file);
		}

		$paragraph->field_playlist_video[LANGUAGE_NONE][0] = array(
			'fid' => $file->fid,
			'display' => 1,
			'description' => '',
		);

		$filename = 'poster02.jpg';
		$source = drupal_get_path('module', 'video_playlist') . '/test_content/' . $filename;
		$media = file_get_contents($source);
		$dest = 'public://' . $filename;
		$file = file_save_data($media, $dest, FILE_EXISTS_RENAME);

		if ($file) {
			// file_save_upload marks the file as temporary, we need to mark as permanent or it will be cleared from the database in a few hours
			$file->status = FILE_STATUS_PERMANENT;
			file_save($file);
		}

		$paragraph->field_playlist_video_poster[LANGUAGE_NONE][0] = array(
			'fid' => $file->fid,
			'display' => 1,
			'description' => '',
		);

		$paragraph->field_playlist_video_title[LANGUAGE_NONE][0]['value'] = "Espresso Lungo";

		$paragraph->field_playlist_video_description[LANGUAGE_NONE][0]['value'] = "A video of Espresso Lungo.";

		$paraWrap->save();
		$paragraph->save();
	}

	drupal_set_message($message);
}

/**
 * Get fields list callback function().
 */
function video_playlist_get_fields_list($form, $form_state) {
	$contentType = !empty($form_state['values']['video_playlist_custom_content_type']) ? $form_state['values']['video_playlist_custom_content_type'] : "page";
	$instances = field_info_instances('node', $contentType);
	$options = '';
	foreach($instances as $key => $instance) {
		$options .= '<option value="' . $key . '">' . $instance['label'] . '</option>';
	};

	$html = '<select id="edit-video-playlist-custom-paragraph-field" name="video_playlist_custom_paragraph_field" class="form-select">' . $options . '</select>';

	return $html;
}

/**
 * Video playlist full setup callback function().
 */
function video_playlist_full_setup($form, $form_state) {
	$paraField = $form_state['values']['para_field_mn'];

	video_playlist_create_content_type($paraField);
	video_playlist_create_paragraph_bundles();
	video_playlist_create_test_content('video_playlist', $paraField);

	// Set message.
	$commands[] = ajax_command_remove('#messages-wrap');
 	$commands[] = ajax_command_after('#branding', '<div id="messages-wrap">' . theme('status_messages') . '</div>');

	return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Video playlist custom setup callback function().
 */
function video_playlist_custom_setup($form, $form_state) {
	$contentType = $form_state['values']['video_playlist_custom_content_type'];
	$paraField = $form_state['values']['para_field_mn'];

	video_playlist_create_node_fields($contentType, $paraField);
	video_playlist_create_paragraph_bundles();
	video_playlist_create_test_content($contentType, $paraField);

	// Set message.
	$commands[] = ajax_command_remove('#messages-wrap');
 	$commands[] = ajax_command_after('#branding', '<div id="messages-wrap">' . theme('status_messages') . '</div>');

	return array('#type' => 'ajax', '#commands' => $commands);
}