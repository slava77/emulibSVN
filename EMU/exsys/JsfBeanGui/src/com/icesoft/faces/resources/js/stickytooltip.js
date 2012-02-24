/* Sticky Tooltip script (v1.0)
* Created: Nov 25th, 2009. This notice must stay intact for usage 
* Author: Dynamic Drive at http://www.dynamicdrive.com/
* Visit http://www.dynamicdrive.com/ for full source code
* Modified for RR by Valdas
*/


var stickytooltip={
    
    tooltipoffsets: [20, -30], //additional x and y offset from mouse cursor for tooltips
    fadeinspeed: 500, //duration of fade effect in milliseconds
    rightclickstick: true, //sticky tooltip when user right clicks over the triggering element (apart from pressing "s" key) ?
    stickybordercolors: ["#BBBBBB", "#000000"], //border color of tooltip depending on sticky state
    stickynotice1: "Press \"s\" to sticky box",
    stickynotice2: "Click outside this box to hide it", //customize tooltip status message

    //***** NO NEED TO EDIT BEYOND HERE

    isdocked: false,

    positiontooltip: function(jQuery, $tooltip, e) {
        var x = e.pageX + this.tooltipoffsets[0];
        var y = e.pageY + this.tooltipoffsets[1];
        var tipw = $tooltip.outerWidth(), tiph = $tooltip.outerHeight();
        x = (x + tipw > jQuery(document).scrollLeft() + jQuery(window).width()) ? x - tipw - (stickytooltip.tooltipoffsets[0] * 2) : x;
        y = (y + tiph > jQuery(document).scrollTop() + jQuery(window).height()) ? jQuery(document).scrollTop() + jQuery(window).height() - tiph - 10 : y;
        $tooltip.css( {
            left: x,
            top: y
        })
    },
	
    showbox: function(jQuery, $tooltip, e){
        $tooltip.fadeIn(this.fadeinspeed)
        this.positiontooltip(jQuery, $tooltip, e)
    },

    hidebox: function(jQuery, $tooltip){
        if (!this.isdocked) {
            $tooltip.stop(false, true).hide()
            $tooltip.css({
                borderColor: this.stickybordercolors[0]
            }).find('.stickystatus:eq(0)').css({
                background:this.stickybordercolors[0]
                }).html(this.stickynotice1);
        }
    },

    docktooltip: function(jQuery, $tooltip, e) {
        this.isdocked = true;
        $tooltip.css({
            borderColor: this.stickybordercolors[1]
        }).find('.stickystatus:eq(0)').css({
            background: this.stickybordercolors[1]
            }).html(this.stickynotice2)
    },


    attach: function(e, targetselector, tipid) {
        
        jQuery(document).ready(function(jQuery) {

            var $target = jQuery(targetselector);
            if ($target.length == 0) return;

            var $ttText = jQuery('#'+tipid);
            if (!$ttText.html()) return;
            if ($ttText.html().trim().length == 0) return;

            $target.data("tooltip", tipid);

            var $tooltip=jQuery("#divTooltip");
            if ($tooltip.length == 0) {
                $tooltip = jQuery('<div id="divTooltip" class="stickytooltip"><div id="divTooltipText" class="divTooltipText"></div><div class="stickystatus">' + stickytooltip.stickynotice1 + '</div></div>');
                jQuery('body').append($tooltip);
                $tooltip.hide();
            }
            
            var $tooltipText = jQuery("#divTooltipText", $tooltip);

            $target.bind('mouseenter', function(e) {
                if (!stickytooltip.isdocked) {
                    var ttt = jQuery('#' + jQuery(this).data('tooltip')).html();
                    if (ttt.length > 0) {
                        $tooltipText.html(ttt);
                        //$tooltip.show();
                        stickytooltip.showbox(jQuery, $tooltip, e);
                    }
                }
            });

            $target.bind('mouseleave', function(e){
                stickytooltip.hidebox(jQuery, $tooltip);
            });
                        
            $target.bind('mousemove', function(e){
                if (!stickytooltip.isdocked) {
                    stickytooltip.positiontooltip(jQuery, $tooltip, e);
                }
            });

            $tooltip.bind("mouseenter", function(){
                stickytooltip.hidebox(jQuery, $tooltip);
            });

            $tooltip.bind("click", function(e) {
                e.stopPropagation();
            });
                        
            jQuery(this).bind("click", function(e) {
                if (e.button == 0){
                    stickytooltip.isdocked = false;
                    stickytooltip.hidebox(jQuery, $tooltip);
                }
            });
  
            jQuery(this).bind('keypress', function(e){
                var keyunicode = e.charCode || e.keyCode;
                if (keyunicode == 115 && jQuery(':visible', $tooltip).length > 0) { //if "s" key was pressed
                    stickytooltip.docktooltip(jQuery, $tooltip, e);
                }
            });

            if (!stickytooltip.isdocked) {
                var ttt = $ttText.html();
                if (ttt.length > 0) {
                    $tooltipText.html(ttt);
                    //$tooltip.show();
                    stickytooltip.showbox(jQuery, $tooltip, e);
                }
            }

        })

    }
}

