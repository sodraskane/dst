<?php
use \Psr\Http\Message\ServerRequestInterface as Request;
use \Psr\Http\Message\ResponseInterface as Response;

/*
 * List all delgates
 */
$app->get('/api/speakers', function (Request $request, Response $response) {
    $speakers = [
        '17' => [
            'delegate' => '42',
            'name' => 'Johan Holmberg',
            'group' => 'Månstorp'
        ],
        '18' => [
            'delegate' => '13',
            'name' => 'Håkan Kvist',
            'group' => 'Drottningstaden'
        ]
    ];

    if ($request->getHeader('Accept')[0] == 'application/json') {
        return $response
            ->withHeader('Content-Type', 'application/json')
            ->write(json_encode($speakers, JSON_UNESCAPED_UNICODE));
    } else {
        return $this->view->render($response, 'list.twig', [
            'items' => $speakers,
            'title' => 'speakers'
        ]);

        $output = "This is where we will list all the delegates";
        $response->getBody()->write($output);

        return $response;
    }
});

/*
 * Add a speaker
 */
$app->post('/api/speakers', function (Request $request, Response $response) {
    $speaker = [
            'speaker' => '17',
            'delegate' => '42'
        ];
    $response->getBody()->write(json_encode($speaker, JSON_UNESCAPED_UNICODE))
            ->withStatus(201)
            ->withHeader('Location:', '/api/speakers/17');

    return $response;
});

/*
 * Retrieve a single speaker
 */
$app->get('/api/speakers/{speaker}', function (Request $request, Response $response, $args) {
    $speaker = [
            'speaker' => $args['speaker'],
            'delegate' => '42',
            'name' => 'Johan Holmberg',
            'group' => 'Månstorp'
        ];
    $response->getBody()->write(json_encode($speaker, JSON_UNESCAPED_UNICODE));

    return $response;
});

/*
 * Update a speaker
 */
$app->put('/api/speakers/{speaker}', function (Request $request, Response $response, $args) {
    $response->withStatus(204);

    return $response;
});

/*
 * Update a speaker
 */
$app->patch('/api/speakers/{speaker}', function (Request $request, Response $response, $args) {
    $response->withStatus(204);

    return $response;
});

/*
 * Delete a speaker
 */
$app->delete('/api/speakers/{speaker}', function (Request $request, Response $response, $args) {
    $response->withStatus(204);

    return $response;
});
