// $Id$
/**
 * @file
 * Admin interface javascript plugin.
 */

var httpCodeUpdateSelection = function () {
  var code = $('.uuid-resolver-http-code-custom').val();
  var found = false;
  $('input.uuid-resolver-http-code-default').each(function (index) {
    if (!found) {
      if ($(this).val() == code) {
        $(this).attr('checked', 'checked');
        found = true;
      }
    }
  });
  if (!found) {
    $('input.uuid-resolver-http-code-default[value=-1]').attr('checked', 'checked');
  }
}

var hideCleanNotify = function (event) {
  var cleanNotify = $('#path-clean-notify');
  if (event.which != 13 && cleanNotify.is(':visible')) {
    cleanNotify.fadeOut(200);
  }
  return true;
}

$(document).ready(function () {
  var resolverOverviewEnabled = $('#uuid-resolver-overview-table input:checkbox.checkbox-enabled');
  if (resolverOverviewEnabled.size() > 0) {
    var resolverForm = $('#uuid-resolver-overview-form');
    resolverOverviewEnabled.change(function () {
      resolverForm.submit();
    });
    $('#uuid-resolver-actions').hide();
  }
  
  var httpCodeCustom = $('.uuid-resolver-http-code-custom');
  if (httpCodeCustom.size() > 0) {
    httpCodeCustom.focus(httpCodeUpdateSelection);
    httpCodeCustom.keyup(httpCodeUpdateSelection);
    httpCodeCustom.change(httpCodeUpdateSelection);
  }

  var resolverSettingsForm = $('.uuid-resolver-resolver-settings-form');
  if (resolverSettingsForm.size() > 0) {
    var cleanNotify = $('<span id="path-clean-notify">(Path cleaned. Submit again to save.)</span>')
      .css('color', '#888').css('fontSize', '87%')
      .css('marginLeft', '20px').css('display', 'inline').hide();
    cleanNotify.insertAfter($('.uuid-resolver-resolver-settings-form #edit-base-path-wrapper span.field-suffix'));

    var editBasePath = $('#edit-base-path');
    editBasePath.keydown(hideCleanNotify);
    editBasePath.focus(hideCleanNotify);

    resolverSettingsForm.submit(function () {
      var origPath = editBasePath.val();
      if (origPath != '') {
        var newPath = origPath.replace(/\s+/g, '').replace(/^\/+|\/+$/g, '').replace(/\/{2,}/g, '/');
        if (newPath != origPath) {
          editBasePath.val(newPath);
          if (!cleanNotify.is(':visible')) {
            cleanNotify.fadeIn('800');
          }
          return false;
        }
      }
      return true;
    });
  }
});