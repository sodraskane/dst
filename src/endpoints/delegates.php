<?php

/*
 * List all delgates
 */
$app->get('/api/delegates', function (Request $request, Response $response) {
    $output = "This is where we will list all the delegates";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Add a delegate
 */
$app->post('/api/delegates', function (Request $request, Response $response) {
    $output = "This is where we will add a delegate";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Retrieve a single delegate
 */
$app->get('/api/delegates/{id}', function (Request $request, Response $response) {
    $output = "This is where we will retrieve a delegate";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Update a delegate
 */
$app->put('/api/delegates/{id}', function (Request $request, Response $response) {
    $output = "This is where we will update a delegate";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Update a delegate
 */
$app->patch('/api/delegates/{id}', function (Request $request, Response $response) {
    $output = "This is where we will list update/patch a delegate";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Delete a delegate
 */
$app->delete('/api/delegates/{id}', function (Request $request, Response $response) {
    $output = "This is where we will delete a delegate";
    $response->getBody()->write($output);

    return $response;
});
