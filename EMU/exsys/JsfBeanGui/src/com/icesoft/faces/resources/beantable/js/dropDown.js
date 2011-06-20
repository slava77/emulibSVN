(function($){
    $.fn.extend({
        
        dropDown: function(p) {
         
            var panel = p;
         
            return this.each(function() {
                
                var control = $(this);
                
                control.click(
                    function () {
                        panel.trigger('controlClick', [ control ]); 
                        return false;
                    }
                );
                
                panel
                    .hover(function() {
                        jQuery(this).data("mouse_inside", true);
                    }, function() {
                        jQuery(this).data("mouse_inside", false);
                    })
                    .bind('controlClick', function(event, control) {
                        var p = jQuery(this);
                        if (!p.is(':visible')) {
                            var c = jQuery(control);
                            p.data("control_click", true);

                            var sTop = 0, sLeft = 0;
                            c.parents().each(function () {
                                sTop  += jQuery(this).scrollTop();
                                sLeft += jQuery(this).scrollLeft();
                            });

                            var left = c.position().left + sLeft;
                            var leftLimit = jQuery(window).width() + sLeft;
                            if (left + p.outerWidth(true) > leftLimit) {
                                left = leftLimit - p.outerWidth(true);
                            }

                            var top  = c.position().top + c.outerHeight(true) + sTop;

                            p.css("top", top).css("left", left).show('fast');

                        } else {
                            p.hide('fast');
                        }
                    })
                    .css("position", "absolute");

                jQuery("body").mouseup(function() {
                    if (panel.is(':visible') && !panel.data("mouse_inside")) {
                        panel.hide('fast');
                    }
                });

                ice.onBeforeUpdate(function() {
                    if (panel.is(':visible')) {
                        if (!panel.data("control_click")) {
                            panel.hide('fast');
                        } else {
                            panel.data("control_click", false);
                        }
                    }
                });
                
            });
        }
    });
})(jQuery);