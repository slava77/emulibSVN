$(document).ready(function() {

    // Dialogs

    var setButtonDialog = function(id, title, wrapToForm) {
        var json = null;
        try {
            json = $.parseJSON(getCookie(id));
        } catch (e) { }

        var autoOpen = 0;
        var pos;
        if (json != null) {
            autoOpen = json.show;
            if (json.left < 0 || json.top < 0) {
                pos = "center";
            } else {
                pos = [ json.left , json.top ]
            }
        } else { 
            autoOpen = 0;
            pos = "center";
        }
        if (id == "drillthrough") {
            $("#" + id).dialog({
                autoOpen:  (autoOpen == 0  ? false : true),
                resizable: false,
                title: title,
                width: 800,
                maxWidth: 800,
                position: pos
            });
        }
        else {
            $("#" + id).dialog({
                autoOpen: (autoOpen == 0 ? false : true),
                resizable: false,
                title: title,
                width: 'auto',
                position: pos
            });
        }

        if (wrapToForm) {
            $("#" + id).wrap("<form/>");
        }

        var bmButton;
        if (id == "drillthrough") {
            $("#drill").click(function() {
                $("#" + id).dialog({
                    width: 600
                });
                $("#" + id).dialog("open");
            })
        } else  {
            bmButton = $("a[href=" + id + "]");
            $(bmButton).removeAttr("href");
            $(bmButton).click(function() {
                $("#" + id).dialog("open");
            });
        }
    };

    setButtonDialog("mdxedit", "MDX Query Editor", true);
    setButtonDialog("navi", "Navigator", true);
    setButtonDialog("sort", "Sort Options", true);
    setButtonDialog("chartprop","Chart Properties", true);
    setButtonDialog("printprop","Print Properties", true);
    setButtonDialog("selectprop","Select Properties", true);
    setButtonDialog('bookmarks', 'Bookmarks Management', false);
    setButtonDialog('cubes', 'Facts / Cubes', false);
    setButtonDialog("drillthrough", "Drill Through", true);
    $("#bookmarkName").dialog({
        autoOpen: false,
        resizable: false,
        title: "Bookmark Title",
        width: 'auto',
        modal: true
    });
   
    // Bookmarks dialog

    var reloadBookmarks = function() {
        var title = $("#bookmarkList").val();
        $("#bookmarkList").empty().append("<option value=''>New..</option>");
        $.ajax({
            url: 'bookmarks',
            data: "action=list",
            dataType: "json",
            success: function(data) {
                $.each(data, function(i, o) {
                    $("#bookmarkList").append("<option value='" + o + "' " + (o == title ? "selected" : "") + ">" + o + "</option>");
                });
            }
        });
    };

    var bookmarkAction = function(action, title) {
        $.ajax({
            url: 'bookmarks',
            dataType: "json",
            async: false,
            data: "action=" + action + "&title=" + title,
            success: function() {
                reloadBookmarks();
            }
        });
    };

    $("#bookmarks button[title=Save]").click(function() {
        var title = $("#bookmarkList").val();
        if (title != "") {
            bookmarkAction("save", title);
        } else {
            $("#bookmarkName").dialog("open");
        }
    });

    $("#bookmarkName button[title=Cancel]").click(function() {
        $("#bookmarkName").dialog("close");
    });

    $("#bookmarkName button[title=OK]").click(function() {
        var title = $("#bookmarkName input").val();
        if (title != "") {
            bookmarkAction("save", title);
            $("#bookmarkName").dialog("close");
        }
    });

    $("#bookmarks button[title=Load]").click(function() {
        var title = $("#bookmarkList").val();
        if (title != "") {
            bookmarkAction("load", title);
            $("#bookmarks").dialog("close");
            window.location.href = "index.jsp";
        }
    });

    $("#bookmarks button[title=Delete]").click(function() {
        var title = $("#bookmarkList").val();
        if (title != "") {
            bookmarkAction("delete", title);
        }
    });

    reloadBookmarks();

    $.ajax({
        url: 'action',
        data: "id=cubes",
        dataType: "json",
        success: function(data) {
            var ul = $("#cubes").append("<ul/>");
            $.each(data, function(i, o) {
                $(ul).append("<li><a href=\"action?id=select&cube=" + o + "\">" + o + "</a></li>");
            });
        }
    });

});

$(window).unload(function() {

    var setJsonCookie = function(diag, pshow) {

        var pid = diag.id;
        var pleft = -1, ptop = -1;
        if (pshow == 1) {
            pleft = $(diag).offset().left;
            pleft = (pleft == 0 ? pleft : pleft - 4.5);
            ptop = $(diag).offset().top;
            ptop = (ptop == 0 ? ptop : ptop - 35.5);
        } else {
            try {
                var cjson = $.parseJSON(getCookie(pid));
                if (cjson != null) {
                    pleft = cjson.left;
                    ptop = cjson.top;
                }
            } catch (e) { }
        }

        //alert(pid + "," + pshow + "," + pleft + "," + ptop);
        
        var json = {
            show:  pshow,
            left:  pleft,
            top:   ptop
        };
        setCookie(pid, $.toJSON(json), 1);
    };

    $(".ui-dialog-content:visible").each(function () {
        setJsonCookie(this, 1);
    });

    $(".ui-dialog-content:hidden").each(function () {
        setJsonCookie(this, 0);
    });

});
                      