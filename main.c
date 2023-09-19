#include <raylib.h>
#include <math.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 1000
#define GRAVITY 800
#define JUMP_FORCE 450
#define PLAYER_SPEED 200
#define PARTICLE_SPEED 1000
#define MAX_PARTICLES 10000 
#define MAX_PLATS 1000
int platformCount = 10;

typedef struct {
    Vector2 position;
    Vector2 velocity;
} Player;

typedef struct {
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    int moveTimer; 
    bool active;
} Platform;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool active;
} Particle;

Particle *particles = NULL;
int particleCount = 0;


void CreateParticle(Vector2 position, Vector2 velocity) {
        particles = (Particle *)realloc(particles, (particleCount + 1) * sizeof(Particle));
        particles[particleCount].position.y = position.y+50;
        particles[particleCount].position.x = position.x+25;
        particles[particleCount].velocity = velocity;
        particles[particleCount].active = true;
        particleCount++;
}

bool CheckCollisionBottom(Vector2 playerPos, Vector2 platformPos, Vector2 platformSize) {
    return (playerPos.y + 50 >= platformPos.y-5 && playerPos.y + 50 <= platformPos.y - 5 + platformSize.y &&
            playerPos.x + 50 >= platformPos.x && playerPos.x <= platformPos.x + platformSize.x);
}

void initPlatforms(Platform platforms[MAX_PLATS]){
    for (int i = 0; i < platformCount; i++) {
        platforms[i].position.x = rand() % (SCREEN_WIDTH-100);
        platforms[i].position.y = rand() % (SCREEN_HEIGHT-300) + 100;
        platforms[i].size = (Vector2){rand() % 100 + 50, 10};
        platforms[i].velocity = (Vector2){(float)(rand() % 201 - 100), 0}; 
        platforms[i].moveTimer = rand() % 100;
        platforms[i].active = true;
    }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "John's Game github.com/JohnLins (Made with 60% ChatGPT code)");

    Player player;
    player.position = (Vector2){SCREEN_WIDTH/2+25, SCREEN_HEIGHT};
    player.velocity = (Vector2){0, 0};

    SetTargetFPS(60);
    InitAudioDevice();
    Sound jumpSound = LoadSound("assets/jump.mp3");
    Sound landSound = LoadSound("assets/land.wav");
    Sound WIN = LoadSound("assets/WIN.mp3");
    Sound lose = LoadSound("assets/lose.mp3");
    
   

    Platform platforms[MAX_PLATS];
    initPlatforms(platforms);

    bool gameover = false; 
    bool won = false;
    bool wasInAir = false;

    while (!WindowShouldClose()) {
        if (!gameover) {
            if (IsKeyDown(KEY_RIGHT)) {
                player.velocity.x = PLAYER_SPEED;
            } else if (IsKeyDown(KEY_LEFT)) {
                player.velocity.x = -PLAYER_SPEED;
            } else {
                player.velocity.x = 0;
            }
            
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)) {
                PlaySound(jumpSound); 
                wasInAir = true;
                player.velocity.y = -JUMP_FORCE;
                for (int i = 0; i < 20; i++) {
                    Vector2 particleVelocity = {(float)(rand() % 201 - 100), -PARTICLE_SPEED}; 
                    CreateParticle(player.position, particleVelocity);
                }
            }

         
            if (player.position.y <= SCREEN_HEIGHT - 100) {
                player.velocity.y += GRAVITY * GetFrameTime();
            } else {
                player.position.y = SCREEN_HEIGHT - 100;
            }
            
           
            player.position.x += player.velocity.x * GetFrameTime();
            player.position.y += player.velocity.y * GetFrameTime();

            for (int i = 0; i < platformCount; i++) {
                if (platforms[i].active) {
                    platforms[i].position.x += platforms[i].velocity.x * GetFrameTime();
                    platforms[i].moveTimer--;
                    if (platforms[i].moveTimer <= 0) {
                        platforms[i].velocity.x = (float)(rand() % 201 - 100);
                        platforms[i].moveTimer = rand() % 100;
                    }
                    
                    if (platforms[i].position.x > SCREEN_WIDTH)
                        platforms[i].position.x = -platforms[i].size.x;
                    else if (platforms[i].position.x < -platforms[i].size.x)
                        platforms[i].position.x = SCREEN_WIDTH;
                }
            }

         
            for (int i = 0; i < platformCount; i++) {
                if (platforms[i].active) {
                    if (CheckCollisionBottom(player.position, platforms[i].position, platforms[i].size) && player.velocity.y > 0) {
                        player.velocity.y = 0; 
                        player.position.y = platforms[i].position.y - 50 - 5; 
                        if(wasInAir){
                            PlaySound(landSound);
                        }
                        wasInAir = false;
                    }
                }
            }
            
            
           
            if(player.position.x <= 0 ){
                player.velocity.x=0;
                player.position.x=0;
            }
            if(player.position.x + 50 >= SCREEN_WIDTH){
                player.velocity.x=0;
                player.position.x=SCREEN_WIDTH-50;
            }
            
            if(player.position.y <= 10){
                PlaySound(WIN);
                won = true;
                platformCount += 5;
                gameover = true;
            }
            
      
            
        for (int i = 0; i < platformCount; i++) {
            if (platforms[i].active) {
                if (player.position.y <= platforms[i].position.y + platforms[i].size.y &&
                    player.position.y +50 >= platforms[i].position.y &&
                    player.position.x + 50 >= platforms[i].position.x &&
                    player.position.x <= platforms[i].position.x + platforms[i].size.x) {
                        player.velocity.y = 0;
                        player.velocity.x = 0;
                        platforms[i].active = false;
                        PlaySound(lose);
                        gameover = true; 
                }
            }
        }
        
     
            
        }
        
       


        for (int i = 0; i < particleCount; i++) {
            if (particles[i].active) {
                particles[i].position.x -= particles[i].velocity.x * GetFrameTime() + rand()%(SCREEN_WIDTH/100)-(SCREEN_WIDTH/200);
                particles[i].position.y -= particles[i].velocity.y * (rand()%100)/100.0 * GetFrameTime();
                if(particles[i].velocity.y>0){
                    particles[i].velocity.y += 5;
                }
                
                if (particles[i].position.x < 0 || particles[i].position.x > SCREEN_WIDTH ||
                    particles[i].position.y < 0 || particles[i].position.y > SCREEN_HEIGHT) {
                    particles[i].active = false;
                }
            }
        }

        
        BeginDrawing();
        ClearBackground((Color){0,0,255*((SCREEN_HEIGHT-player.position.y)/SCREEN_HEIGHT),255});
         DrawLine(0,10,SCREEN_WIDTH,10, GREEN);
         

        
        DrawRectangleV(player.position, (Vector2){50, 50}, GREEN);

        
        for (int i = 0; i < platformCount; i++) {
            if (platforms[i].active) {
                DrawRectangleV(platforms[i].position, platforms[i].size, RED);
                DrawRectangleV((Vector2){platforms[i].position.x, platforms[i].position.y - 5}, (Vector2){platforms[i].size.x, 5}, RAYWHITE);
            }
        }

       
        for (int i = 0; i < particleCount; i++) {
            if (particles[i].active) {
                int dist = abs(particles[i].position.y - player.position.y);
                DrawCircleV(particles[i].position, 2, (Color){255.0*((SCREEN_HEIGHT-dist)/(float)SCREEN_HEIGHT),255*dist/SCREEN_HEIGHT,255*dist/SCREEN_HEIGHT,255});
            }
        }

       
        if (gameover) {
            if(won){
                DrawText("YOU WON! Press R for the next level", 10, SCREEN_HEIGHT / 2, 30, GRAY);
            } else {
                DrawText("YOU LOST! Press R to restart", 10, SCREEN_HEIGHT / 2, 30, GRAY);
            }
            if (IsKeyPressed(KEY_R)) {
                gameover = false;
                won = false;
                
                 player.position = (Vector2){SCREEN_WIDTH/2+25, SCREEN_HEIGHT};
                 player.velocity = (Vector2){0,0};
                initPlatforms(platforms);
            }
        }
           DrawText(TextFormat("%d PLATFORMS", platformCount), 10, 30, 30, RAYWHITE);

        EndDrawing();
    }

    UnloadSound(jumpSound); 
    UnloadSound(landSound);
    UnloadSound(WIN);
    UnloadSound(lose);
    CloseWindow();

    return 0;
}
