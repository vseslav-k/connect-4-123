#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"
#include "classes/Checkers.h"
#include "classes/Othello.h"
#include "classes/Connect4.h"

namespace ClassGame {
        //
        // our global variables
        //
        Game *game = nullptr;
        bool gameOver = false;
        int gameWinner = -1;
        int aiStatus = 2;
        unsigned int sessions = 0;
        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = nullptr;
        }

        const char* dispAIPlayerStatus(int status){
            switch(status){
                case 0 : return "AI Player: None";
                case 1 : return "AI Player: 1";
                case 2 : return "AI Player: 2";
                case 3 : return "AI Player: Both";
            }
            return "AI Player: ERROR!";
        }

        void getSessions(){
            ImGui::SameLine();
            ImGui::InputScalar("Training Sessions", ImGuiDataType_U32, &sessions);
            sessions = std::max(0U, sessions);
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport();

                //ImGui::ShowDemoWindow();

                ImGui::Begin("Settings");

                if (gameOver) {
                    ImGui::Text("Game Over! ");
                    ImGui::SameLine();
                    ImGui::Text("Winner: %d", gameWinner);
                }
                if (sessions) {
                    ImGui::Text("Sessions Left: %d", sessions);
                }
                if(gameOver && sessions > 0){
                        --sessions;
                        game->stopGame();
                        game->setUpBoard();
                        gameOver = false;
                        gameWinner = -1;
                }
                
                if (!game) {
                    if (ImGui::Button("Start Tic-Tac-Toe")) {
                        game = new TicTacToe();
                        game->setUpBoard();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Start Checkers")) {
                        game = new Checkers();
                        game->setUpBoard();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Start Othello")) {
                        game = new Othello();
                        game->setUpBoard();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Start Connect4")) {
                        game = new Connect4(aiStatus);
                        game->setUpBoard();
                    }
                    
                    if (ImGui::Button(dispAIPlayerStatus(aiStatus))) {
                        aiStatus = (aiStatus+1) % 4;
                    }
                    if(aiStatus == 3){
                        getSessions();
                    }else{
                        sessions = 0;
                    }



                } else {
                    ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                    ImGui::Text("Current Board State: %s", game->stateString().c_str());

                    if (ImGui::Button("Reset Game")) {
                        game->stopGame();
                        game->setUpBoard();
                        gameOver = false;
                        gameWinner = -1;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Back")) {
                        delete game;
                        game = nullptr;
                    }
                }
                ImGui::End();

                ImGui::Begin("GameWindow");
                if (game) {
                    if (game->gameHasAI() && (game->getCurrentPlayer()->isAIPlayer() || game->_gameOptions.AIvsAI))
                    {
                        game->updateAI();
                    }
                    game->drawFrame();
                }
                ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameOver = true;
                gameWinner = winner->playerNumber();
            }
            if (game->checkForDraw()) {
                gameOver = true;
                gameWinner = -1;
            }
        }
}
