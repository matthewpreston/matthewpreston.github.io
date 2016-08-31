$(document).ready(function() {
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

});