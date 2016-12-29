$(document).ready(function() {
  var $window = $(window)

  //Fix divider background-color
  $(".divider").each(function() {
    $(this).css("background-color", $(this).closest(".container-fluid").css("color"));
  });

  //Fix button border-color
  $(".btn-view").each(function() {
    $(this).css("border-color", $(this).closest(".container-fluid").css("color"));
  });

  //Parallax scrolling
  var velocity = 0.3; //Y axis scroll speed
  function parallax() { 
    var pos = $(window).scrollTop();
    var $progCont = $(".program-container");
    var height = $progCont.height();
    $progCont.css("background-position", "50% " + Math.round((height - pos) * velocity) + "px"); 
  }
  parallax();//Set up initial conditions
  $window.bind("scroll", parallax);

  //Change the properties of elements when resizing
  function resizer() {
    //Deal with program welcome header
    var $progCont = $(".program-container");
    $progCont.css("padding", "50px " + Math.round(0.02 * $(window).width() + 3) + "px");

  	//Deal with elements in each main container
    $(".main-container").each(function() {
      //Set up useful variables
      var $cont;
      if ($(this).hasClass("reverse")) {
        $cont = $(this).find(".picture");
      } else {
        $cont = $(this).find(".text");
      }
      var contTop = $cont.position().top;
      var contHeight = $cont.height();
      text = $(this).find(".text");
      var textHeight = text.height();
      var textWidth = text.width();

      //Fix picture positioning
      var $img = $(this).find("img");
      var imgHeight = $img.height();
      var imgPadding;
      if (contTop == 0) {//Side-by-side
        imgPadding = Math.max((textHeight - imgHeight) / 2, 10);
      } else {//On top of each other
        imgPadding = 10;
      }
      $img.css({paddingTop: imgPadding, paddingBottom: imgPadding});

      //Fix "View More" button's heights
      var $button = $(this).find(".btn-view");
      var btnHeight = $button.height();
      var btnWidth = $button.width();
      if (contTop == 0) {//Side-by-side
        $button.css({top: $(this).find(".row").height() - btnHeight - 15, left: 15});
      } else {//On top of each other
        $button.css({top: textHeight - btnHeight, left: (textWidth - btnWidth) / 2});
      }
    });
  }
  resizer();//Set up initial conditions
  $window.bind("resize", resizer);
});