jQuery(document).ready(function ($) {
if(!$("body").hasClass('page-node-edit')){
    $('.vjs-playlist .vjs-playlist-item').on('click touchstart', function () {

        var index = $(this).index();

        $(this).closest(".video-playlist-wrapper").find(".video-data-description-wrapper").addClass("hide");
        $(this).closest(".video-playlist-wrapper").find(".video-data-description-wrapper").eq(index).removeClass("hide");
    });
}
});
