<?php
use \Psr\Http\Message\ServerRequestInterface as Request;
use \Psr\Http\Message\ResponseInterface as Response;

/*
 * List all delgates
 */
$app->get('/api/speakers', function (Request $request, Response $response) {
    $speakers = Speaker::orderBy('id', 'asc')->get();
    foreach ($speakers as $speaker) {
         $speaker->delegate = Delegate::find($speaker->delegate);
    }
    return $response->withJson($speakers);
});

/*
 * Add a speaker
 */
$app->post('/api/speakers', function (Request $request, Response $response) {
    $data = $request->getParsedBody();
    $speaker = new Speaker();
    $speaker->delegate = $data['delegate'];

    $speaker->save();

    return $response->withJson($speaker)
            ->withStatus(201)
            ->withHeader('Location:', '/api/speakers/' + $speaker->speaker);
});

/*
 * Retrieve a single speaker
 */
$app->get('/api/speakers/{speaker}', function (Request $request, Response $response, $args) {
    $speaker = Speaker::find($args['speaker']);
    if ($speaker != null) {
        $speaker->delegate = Delegate::find($speaker->delegate);
        $response = $response->withJson($speaker);
    } else {
        $response = $response->withStatus(404);
    }
    return $response;
});

/*
 * Update a speaker
 */
$app->put('/api/speakers/{speaker}', function (Request $request, Response $response, $args) {
    $data = $request->getParsedBody();
    $speaker = Speaker::find($args['speaker']);
    if (!$speaker) {
        return $response->withStatus(404);
    }
    $speaker->id = $data['id'] ?: $speaker->id;
    $speaker->delegate = $data['delegate'] ?: $speaker->delegate;

    $speaker->save();

    return $response->withStatus(204);
});

/*
 * Delete a speaker
 */
$app->delete('/api/speakers/{speaker}', function (Request $request, Response $response, $args) {
    $speaker = Speaker::find($args['speaker']);
    if ($speaker) {
         $speaker->delete();
    }

    return $response->withStatus(204);
});
