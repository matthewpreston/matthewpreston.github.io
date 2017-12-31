var MINIMIZED_MAX_CODE_LINES = 10;

$(function() {
	var $window = $(window);
	var $navbar = $(".navbar");
	var $wrapper = $(".wrapper");
	var $footer = $("footer");
	var $article = $("article");
	var $aside = $("aside");
	var $sidebar = $aside.find(".sidebar");
	var $allProgBtn = $aside.find("#all-programs");
	var $topBtn = $aside.find("#top");
	var navbarPadTop = parseInt($navbar.css("paddingTop"));
	var navbarPadBot = parseInt($navbar.css("paddingBottom"));
	var navbarHeight = navbarPadTop + $navbar.height() + navbarPadBot;
	var wrapperPadTop = parseInt($wrapper.css("paddingTop"));
	var wrapperPadBot = parseInt($wrapper.css("paddingBottom"));
	var allProgBtnPadTop = parseInt($allProgBtn.css("paddingTop"));
	var allProgBtnPadBot = parseInt($allProgBtn.css("paddingBottom"));
	var allProgBtnHeight = allProgBtnPadTop + $allProgBtn.height() + allProgBtnPadBot;
	var allProgBtnBorderWidth = parseInt($allProgBtn.css("borderTopWidth")) * 2;
	var topBtnPadTop = parseInt($topBtn.css("paddingTop"));
	var topBtnPadBot = parseInt($topBtn.css("paddingBottom"));
	var topBtnHeight = topBtnPadTop + $topBtn.height() + topBtnPadBot;
	var topBtnBorderWidth = parseInt($topBtn.css("borderTopWidth")) * 2;

	//Fix sidebar whilst scrolling
	function stickySidebar() {
		var windowHeight = $window.height();
		var windowScrollPos = $window.scrollTop();
		var articleTop = $article.position().top;
		var asideTop = $aside.position().top;
		var footerPosTop = $footer.position().top;

		if (articleTop == asideTop) {//Not squished, so sidebar is shown
			//Calculate new sidebar position
			$sidebar.css("top", Math.max(navbarHeight - windowScrollPos, 0) + wrapperPadTop);
			//Calculate new button positions
			var btnOffset = Math.max(windowHeight + windowScrollPos - footerPosTop, 0);
			$allProgBtn.css("top", windowHeight - allProgBtnHeight - allProgBtnBorderWidth - btnOffset - wrapperPadBot);
			$topBtn.css("top", windowHeight - topBtnHeight - topBtnBorderWidth - btnOffset - wrapperPadBot);
		}
	}
  stickySidebar();
	$window.bind("scroll", stickySidebar);

	//Fix sidebar while resizing
	function resizer() {
		var articleTop = $article.position().top;
		var asideTop = $aside.position().top;
		if (articleTop == asideTop) {//Not squished, so sidebar is shown
			$sidebar.show();
			$allProgBtn.show();
			$topBtn.show();
			stickySidebar(); //Fix sidebar positioning
		} else {//Squished; hide
			$sidebar.hide();
			$allProgBtn.hide();
			$topBtn.hide();
		}
	}
  resizer();
	$window.bind("resize", resizer);

	// Limit number of lines displayed by code blocks (i.e. no. of list items)
	var codeBlocks = $.merge($(".program"), $(".terminal"));
	$.each($(codeBlocks), function(index, elem) {
		var listElems = $("li", elem);
		if (listElems.length <= MINIMIZED_MAX_CODE_LINES) {
			// Small block of code, thus hide the button
			$(elem).find(".code-btn").css("display", "none")
		} else {
			// Larger block of code, thus hide excess code
			$.each(listElems, function(index, elem) {
				if (index >= MINIMIZED_MAX_CODE_LINES)
					$(elem).addClass("display-none");
			});
		}
	});

	// Button controls whether to expand their respective code block to show
	// all their list elements
	$.each($(".code-btn"), function(index, elem) {
		$(elem).click(function() {
			var target = $(this).attr("data-target");
			if ($(this).html() == "More") { // Display all list elements
				$.each($(target), function(index, elem) {
					$.each($("li", elem), function(index, elem) {
							$(elem).removeClass("display-none");
					});
				});
				$(this).html("Less");
			} else { // Hide overflowing list elements
				$.each($(target), function(index, elem) {
					$.each($("li", elem), function(index, elem) {
						if (index >= MINIMIZED_MAX_CODE_LINES)
							$(elem).addClass("display-none");
					});
				});
				$(this).html("More");
			}
			stickySidebar();	// To fix sidebar when text becomes hidden/unhidden
		});
	});

	// Fire off functions to fix layout upon user loading page
	stickySidebar();
	resizer();
});