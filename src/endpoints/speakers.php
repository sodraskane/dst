<?php

/*
 * List all delgates
 */
$app->get('/api/speakers', function (Request $request, Response $response) {
    $output = "This is where we will list all the speakers";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Add a speaker
 */
$app->post('/api/speakers', function (Request $request, Response $response) {
    $output = "This is where we will add a speaker";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Retrieve a single speaker
 */
$app->get('/api/speakers/{id}', function (Request $request, Response $response) {
    $output = "This is where we will retrieve a speaker";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Update a speaker
 */
$app->put('/api/speakers/{id}', function (Request $request, Response $response) {
    $output = "This is where we will update a speaker";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Update a speaker
 */
$app->patch('/api/speakers/{id}', function (Request $request, Response $response) {
    $output = "This is where we will list update/patch a speaker";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Delete a speaker
 */
$app->delete('/api/speakers/{id}', function (Request $request, Response $response) {
    $output = "This is where we will delete a speaker";
    $response->getBody()->write($output);

    return $response;
});
