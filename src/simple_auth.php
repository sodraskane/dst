<?php

namespace Dst;

class SimpleAuth {

    var $users;
    var $logged_in = true;

    function __construct($users) {
        $this->users = $users;
    }

    function isLoggedIn() {
        return $this->logged_in;
    }

    function authenticate($user, $pass) {
        $this->logged_in = isset($this->users[$user]) &&
                $this->users[$user] == $pass;
    }
}
