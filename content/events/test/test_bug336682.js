/*
 * Helper functions for online/offline events tests.
 * 
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/licenses/publicdomain/
 */
var gState = 0;
/**
 * After all the on/offline handlers run, 
 * gState is expected to be equal to MAX_STATE.
 */
var MAX_STATE;

function trace(text) {
  var t = text.replace(/&/g, "&" + "amp;").replace(/</g, "&" + "lt;") + "<br>";
  //document.getElementById("display").innerHTML += t;
}

// window.ononline and window.onclick shouldn't work
// Right now, <body ononline=...> sets window.ononline (bug 380618)
// When these start passing, be sure to uncomment the code inside if(0) below.
todo(typeof window.ononline == "undefined",
     "window.ononline should be undefined at this point");
todo(typeof window.onoffline == "undefined",
     "window.onoffline should be undefined at this point");

if (0) {
  window.ononline = function() {
    ok(false, "window.ononline shouldn't be called");
  }
  window.onoffline = function() {
    ok(false, "window.onclick shouldn't be called");
  }
}

/**
 * Returns a handler function for an online/offline event. The returned handler
 * ensures the passed event object has expected properties and that the handler
 * is called at the right moment (according to the gState variable).
 * @param nameTemplate The string identifying the hanlder. '%1' in that
 *                     string will be replaced with the event name.
 * @param eventName 'online' or 'offline'
 * @param expectedStates an array listing the possible values of gState at the
 *                       moment the handler is called. The handler increases
 *                       gState by one before checking if it's listed in
 *                       expectedStates.
 */
function makeHandler(nameTemplate, eventName, expectedStates) {
  return function(e) {
    var name = nameTemplate.replace(/%1/, eventName);
    ++gState;
    trace(name + ": gState=" + gState);
    ok(expectedStates.indexOf(gState) != -1,
       "handlers called in the right order: " + name + " is called, " + 
       "gState=" + gState + ", expectedStates=" + expectedStates);
    ok(e.constructor == Event, "event should be an Event");
    ok(e.type == eventName, "event type should be " + eventName);
    ok(e.bubbles, "event should bubble");
    ok(!e.cancelable, "event should not be cancelable");
    ok(e.target == (document instanceof HTMLDocument
                    ? document.body : document.documentElement),
       "the event target should be the body element");
  }
}

function doTest() {
  netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
  var iosvc = Components.classes["@mozilla.org/network/io-service;1"]
                        .getService(Components.interfaces.nsIIOService2);
  iosvc.manageOfflineStatus = false;
  iosvc.offline = false;
  ok(navigator.onLine, "navigator.onLine should be true, since we've just " +
                       "set nsIIOService.offline to false");

  gState = 0;

  trace("setting iosvc.offline = true");
  iosvc.offline = true;
  trace("done setting iosvc.offline = true");
  ok(!navigator.onLine,
     "navigator.onLine should be false when iosvc.offline == true");
  ok(gState == window.MAX_STATE,
     "offline event: all registered handlers should have been invoked, " +
     "actual: " + gState);

  gState = 0;
  trace("setting iosvc.offline = false");
  iosvc.offline = false;
  trace("done setting iosvc.offline = false");
  ok(navigator.onLine,
     "navigator.onLine should be true when iosvc.offline == false");
  ok(gState == window.MAX_STATE,
     "online event: all registered handlers should have been invoked, " +
     "actual: " + gState);
}
