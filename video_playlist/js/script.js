jQuery(document).ready(function ($) {
if(!$("body").hasClass('page-node-edit')){
    $('.vjs-playlist .vjs-playlist-item').on('click', function () {

        var index = $(this).index();

        $(".video-data-description-wrapper").addClass("hide");
        $(".video-data-description-wrapper").eq(index).removeClass("hide");

    });
}
});
