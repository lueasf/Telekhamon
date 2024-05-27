#include "slider.h"

#include <stdlib.h>





// Constructeur de Slider.
Slider* createSlider(float value) {

    // Malloc le slider.
    Slider* slider = (Slider*)malloc(sizeof(Slider));
    if (!slider) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur lors du malloc de createSlider");
        abort();
    }

    // Le met à zéro.
    slider->value = value;
    slider->locked = 0;

    // Renvoie le slider initialisé.
    return slider;

}



// Destructeur de Slider.
void freeSlider(Slider* slider) {
    free(slider);
}





// Fonction d'update de la valeur du slider.
int sliderUpdate(Slider* slider, unsigned int m_x) {
    if (!slider->locked) return 0;
    float value = slider->value;
    slider->value = ((int)m_x-slider->rect.x)/(float)slider->rect.w;
    if (slider->value < 0.0f) slider->value = 0.0f;
    if (slider->value > 1.0f) slider->value = 1.0f;
    return value != slider->value;
}



// Gestion du clic gauche.
void sliderLeftClick(Slider* slider, unsigned int m_x, unsigned int m_y) {
    if (m_x >= slider->rect.x && m_y >= slider->rect.y && m_x <= slider->rect.x + slider->rect.w
        && m_y <= slider->rect.y + slider->rect.h) slider->locked = 1;
    else slider->locked = 0;
}



// Gestion du clic gauche relâché.
void sliderLeftClickReleased(Slider* slider) {
    slider->locked = 0;
}





// Fonction de redimensionnement.
void sliderResize(Slider* slider, SDL_Rect* rect) {
    slider->rect.x = rect->x;
    slider->rect.y = rect->y;
    slider->rect.w = rect->w;
    slider->rect.h = rect->h;
}
