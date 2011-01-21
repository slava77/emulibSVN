
function guiPollingStart(selector, interval) {
    setTimeout(
        function () { guiPolling(selector, interval) },
        (interval * 1000));
}

function guiPolling(selector, interval) {
    var target = jQuery(selector);
    if (target) {
        target.click();
        guiPollingStart(selector, interval);
    }
}