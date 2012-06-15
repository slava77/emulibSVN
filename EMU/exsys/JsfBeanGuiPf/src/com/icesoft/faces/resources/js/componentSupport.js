
function jqPrefix(selector) {
    return selector.replace(/:/g, '\\:');
}

function addSuffixToolTip(e, o, suffix) {
    if (jQuery(o).data('tooltip') == undefined) { 
        var id = jQuery(o).attr('id');
        var ttId = jqPrefix(id.substring(0, id.lastIndexOf(':')) + ':' + suffix);
        stickytooltip.attach(e, o, ttId); 
    }
}

