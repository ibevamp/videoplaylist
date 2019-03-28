<?php
$node = menu_get_object('node');

$anchorLink = !empty($variables['element']['#object']->field_playlist_video_anchor_link['und'][0]['value'])? $variables['element']['#object']->field_playlist_video_anchor_link['und'][0]['value'] : "";

$heading = !empty($variables['element']['#object']->field_playlist_video_heading['und'][0]['value'])? $variables['element']['#object']->field_playlist_video_heading['und'][0]['value'] : "";
$headingColor = !empty($variables['element']['#object']->field_playlist_video_title_color['und'][0]['rgb']) ? $variables['element']['#object']->field_playlist_video_title_color['und'][0]['rgb'] : 0;
$horizontalBool = !empty($variables['element']['#object']->field_playlist_is_horizontal['und'][0]['value']) ? $variables['element']['#object']->field_playlist_is_horizontal['und'][0]['value'] : 0;

$isHorizontal = 'false';
$class = 'vertical';

if ($horizontalBool == 1) {
    $isHorizontal = 'true';
    $class = "horizontal";
}

$paraID = key($variables['element'][0]['entity']['paragraphs_item']);
//$download    = isset($variables['element'][0]['entity']['paragraphs_item'][$paraID]['field_playlist_video_download']['#items'][0]['value']) ? $variables['element'][0]['entity']['paragraphs_item'][$paraID]['field_playlist_video_download']['#items'][0]['value'] : NULL;

//$description = isset($variables['element'][0]['entity']['paragraphs_item'][$paraID]['field_playlist_video_description']['#items'][0]['value']) ? $variables['element'][0]['entity']['paragraphs_item'][$paraID]['field_playlist_video_description']['#items'][0]['value'] : NULL;
//$title = isset($variables['element'][0]['entity']['paragraphs_item'][$paraID]['field_playlist_video_title'][0]['#markup']) ? $variables['element'][0]['entity']['paragraphs_item'][$paraID]['field_playlist_video_title'][0]['#markup'] : NULL;


?>

<style type="text/css">
      <?php if(!empty($variables['element']['#object']->field_playlist_video_title_color['und'][0]['rgb'])) { ?>
	  <?php echo ".para-".$paraID; ?>.player-container .video-data-description-wrapper .video-data-title,
	  <?php echo ".para-".$paraID; ?>.player-container .vjs-playlist .vjs-playlist-item cite, 
	  <?php echo ".para-".$paraID; ?>.player-container .vjs-playlist .vjs-playlist-item cite.vjs-playlist-name{
		  color:<?php echo $headingColor?> !important;
	  }
	  <?php } ?>
</style>

 <?php if(!empty($variables['element']['#object']->field_playlist_video_anchor_link['und'][0]['value'])){ ?>
	<a name="<?php echo $anchorLink; ?>" id="<?php echo $anchorLink; ?>" class="anchored-link"></a>
<?php } ?>

<div class="wrapper video-playlist-wrapper">
<div class="player-container <?php print $class; ?>  main-preview-player <?php echo "para-".$paraID; ?>">
    <section id="heading" class="up one-up one-up-medium">
         <?php if(!empty($variables['element']['#object']->field_playlist_video_heading['und'][0]['value'])){ ?>
		<header class="text-l module-header">
           <h3 class="headline"><?php print $heading; ?></h3>
        </header>
		<?php } ?>
    </section>
    <section id="one-up-medium" class="up one-up one-up-medium">
        <div class="video-data-section text-m block-list">

            <div class="videojs-wrapper">
			<video
                    id="preview-player"
                    class="video-js vjs-default vjs-big-play-button vjs-big-play-centered"
                    height="300"
                    width="600"
                    controls>

            </video>
			</div>
<?php
$count = 0;
foreach($variables['element'] as $key=>$item){
    if(is_numeric($key)){
        foreach($item['entity']['paragraphs_item'] as $paraKey=>$para){
            $videouri = $para['field_playlist_video']['#items'][0]['uri'];
            $videourl = isset($videouri) ? file_create_url($videouri) : NULL;
            $title = $para['field_playlist_video_title']['#items'][0]['value'];
            $description = $para['field_playlist_video_description']['#items'][0]['value'];
            $download = $para['field_playlist_video_download']['#items'][0]['value'];


            ?>

            <div class="video-data-description-wrapper <?php if($count!=0) print 'hide';?>">

                <div class="text-wrapper">
                    <h3 class="section-header">About this Video
                        <?php if($download){ ?>
                            <a href="<?php print $videourl;?>" download="" class="download-this-article">
                                <span>Download</span>
                                <span class="mck-radial-download-icon"></span>
                            </a>
                        <?php }?>
                    </h3>
                    <h3 class="video-data-title headline"><?php print $title; ?></h3>
                    <div class="video-data-description description"><?php print $description; ?></div>
                </div>


            </div>
<?php
            $count++;
        }
    }
}


?>


        </div>

    </section>
    <div class="playlist-container  preview-player-dimensions vjs-fluid">
        <div class="vjs-playlist">
            <!--
              The contents of this element will be filled based on the
              currently loaded playlist
            -->
        </div>
    </div>
</div>
</div>