#ifndef ONIDEV_PERCEPTRON_H_INCLUDED
#define ONIDEV_PERCEPTRON_H_INCLUDED

#include <vector>

// @todo optimize: https://github.com/cazala/synaptic/wiki/Networks#optimize
// @todo permettre d'utiliser d'autres fonctions d'activation (mais aussi une differente par layer voir par neuron)
// @todo permettre de choisir un gain different pour chaque layer?
// @toto permettre d'ajouter du bruit dans les weights
// @todo permetre de merge plusieurs perceptrons de meme topologie
// @todo move semantic

template<class T>
struct Neuron
{
    T value;
    //T delta;
    T error;
    T derivative;
    std::vector<T> weight;
    std::vector<T> last_weight;
};

// Multi Layer Perceptron
template<class T>
struct Perceptron
{
    std::vector< std::vector<Neuron<T>> > layers;
    T gain = 1;
    
    template<typename... Args>
    Perceptron(Args&&... args);
    
    void randomize();
    void activate();
    void activate(const std::vector<T>& inputs);
    void learn(const std::vector<T>& target, T alpha = 0.9, T eta = 0.2); // (alpha, eta) => learning rate
    void learn(const std::vector<T>& target, const std::vector<T>& alpha, const std::vector<T>& eta);
    
    std::vector<T> output() const;
    
    void computeOutputError(const std::vector<T>& target);
    void computeOutputError(const std::vector<T>& target, T& mse_out, T& mae_out);
    void backPropagateError();
    void adjustWeights(T alpha = 0.9, T eta = 0.2);
    void adjustWeights(const std::vector<T>& alpha, const std::vector<T>& eta);
};

#include "perceptron.inl"

#endif
