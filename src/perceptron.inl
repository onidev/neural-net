
template<class T>
template<typename... Args>
Perceptron<T>::Perceptron(Args&&... args)
{
    static_assert(sizeof...(Args) > 2, "Perceptron requires at least one input, hidden and output layer");
    
    layers.resize(sizeof...(Args));
    int i = 0;
    for(int x: {args...})
    {
        layers[i++].resize(x);
    }
    for(unsigned i=1; i<layers.size(); ++i)
    for(unsigned j=0; j<layers[i].size(); ++j)
    {
        layers[i][j].weight.resize(layers[i-1].size()+1);
        layers[i][j].last_weight.resize(layers[i-1].size()+1);
    }
}

template<class T>
void Perceptron<T>::randomize()
{
    // for each layers
    for(unsigned i=1; i<layers.size(); ++i)
    {
        // for each neuron of the layer
        for(unsigned j=0; j<layers[i].size(); ++j)
        {
            auto& neuron = layers[i][j];
            for(auto& w: neuron.weight)
                w = rand()/static_cast<T>(RAND_MAX) - 0.5;
            for(auto& w: neuron.last_weight)
                w = 0;
        }
    }
}

template<class T>
void Perceptron<T>::activate()
{
    for(int i=1; i<layers.size(); ++i)
    {
        auto& input_layer = layers[i-1];
        for(auto& neuron: layers[i])
        {
            T sum = neuron.weight.back();
            for(unsigned k = 0; k<input_layer.size(); ++k)
            {
                sum += neuron.weight[k] * input_layer[k].value;
            }
            neuron.value = 1.0 / (1.0 + exp(-gain * sum));
            neuron.derivative = neuron.value * (1.0 - neuron.value);
        }
    }
}

template<class T>
void Perceptron<T>::activate(const std::vector<T>& inputs)
{
    for(unsigned i=0; i<layers[0].size() && i<inputs.size(); ++i)
    {
        layers[0][i].value = inputs[i];
    }
    activate();
}


template<class T>
void Perceptron<T>::learn(const std::vector<T>& target, T alpha, T eta)
{
    computeOutputError(target);
    backPropagateError();
    adjustWeights(alpha, eta);
}

template<class T>
void Perceptron<T>::learn(const std::vector<T>& target, const std::vector<T>& alpha, const std::vector<T>& eta)
{
    computeOutputError(target);
    backPropagateError();
    adjustWeights(alpha, eta);
}

template<class T>
std::vector<T> Perceptron<T>::output() const
{
    const auto& t = layers.back();
    std::vector<T> ret(t.size());
    for(unsigned i=0; i<t.size(); ++i)
        ret[i] = t[i].value;
    return ret;
}

template<class T>
void Perceptron<T>::computeOutputError(const std::vector<T>& target)
{
    auto& layer = layers.back();
    for(unsigned i=0; i<layer.size(); ++i)
    {
        auto& neuron = layer[i];
        T delta = target[i] - neuron.value;
        neuron.error = gain * neuron.derivative * delta;
    }
}

template<class T>
void Perceptron<T>::computeOutputError(const std::vector<T>& target, T& mse_out, T& mae_out)
{
    mse_out = 0; // erreur quadratique moyenne
    mae_out = 0; // erreur absolue moyenne
    
    auto& layer = layers.back();
    for(unsigned i=0; i<layer.size(); ++i)
    {
        auto& neuron = layer[i];
        T delta = target[i] - neuron.value;
        neuron.error = gain * neuron.derivative * delta;
        mse_out += delta * delta;
        mae_out += fabs(delta);
    }
    
    mse_out /= target.size();
    mae_out /= target.size();
}

template<class T>
void Perceptron<T>::backPropagateError()
{
    for(int i=layers.size()-1; i>0; --i)
    {
        auto& layer = layers[i];
        auto& input_layer = layers[i-1];
        
        for(unsigned j=0; j<input_layer.size(); j++ )
        {
            T err = 0.0;
            for(auto& neuron: layer)
            {
                err += neuron.weight[j] * neuron.error;
            }
            auto& neuron = input_layer[j];
            neuron.error = gain * neuron.derivative * err;
        }
    }
}

template<class T>
void Perceptron<T>::adjustWeights(T alpha, T eta)
{
    for(unsigned i=1; i<layers.size(); ++i)
    {
        auto& layer = layers[i];
        auto& input_layer = layers[i-1];
        
        for(unsigned j=0; j<layer.size(); ++j)
        {
            auto& neuron = layer[j];
            
            for(unsigned k=0; k<input_layer.size(); ++k)
            {
                T x = eta * input_layer[k].value * neuron.error;
                neuron.weight[k] += x + alpha * neuron.last_weight[k];
                neuron.last_weight[k] = x;
            }
            // update bias
            T x = eta * neuron.error;
            neuron.weight.back() += x + alpha * neuron.last_weight.back();
            neuron.last_weight.back() = x;
        }
    }
}

template<class T>
void Perceptron<T>::adjustWeights(const std::vector<T>& alphas, const std::vector<T>& etas)
{
    // assert(alphas.size() == layers.size()-1);
    for(unsigned i=1; i<layers.size(); ++i)
    {
        auto& layer = layers[i];
        auto& input_layer = layers[i-1];
        
        float alpha = alphas[i-1];
        float eta   = etas[i-1];
        
        for(unsigned j=0; j<layer.size(); ++j)
        {
            auto& neuron = layer[j];
            
            for(unsigned k=0; k<input_layer.size(); ++k)
            {
                T x = eta * input_layer[k].value * neuron.error;
                neuron.weight[k] += x + alpha * neuron.last_weight[k];
                neuron.last_weight[k] = x;
            }
            // update bias
            T x = eta * neuron.error;
            neuron.weight.back() += x + alpha * neuron.last_weight.back();
            neuron.last_weight.back() = x;
        }
    }
}

template<class T>
void Perceptron<T>::save(const std::string& fname) const
{
    std::ofstream f(fname, std::ios::binary);
    char sep = '\n';
    
    f << gain << sep
      << static_cast<uint32_t>(layers.size()) << sep;
    for(const std::vector<Neuron<T>>& layer: layers)
    {
        f << static_cast<uint32_t>(layer.size()) << sep;
        for(const Neuron<T>& neuron: layer)
        {
            f << neuron.value << sep
              << neuron.error << sep
              << neuron.derivative << sep
              << static_cast<uint32_t>(neuron.weight.size()) << sep;
            
            for(const T& w: neuron.weight)
                f << w << sep;
            
            for(const T& w: neuron.last_weight)
                f << w << sep;
        }
    }
}

template<class T>
bool Perceptron<T>::load(const std::string& fname)
{
    std::ifstream f(fname, std::ios::binary);
    
    if(!f.is_open())
        return false;
    
    uint32_t size;
    f >> gain
      >> size;
    layers.resize(size);
    
    for(std::vector<Neuron<T>>& layer: layers)
    {
        f >> size;
        layer.resize(size);
        
        for(Neuron<T>& neuron: layer)
        {
            f >> neuron.value
              >> neuron.error
              >> neuron.derivative
              >> size;
            
            neuron.weight.resize(size);
            for(T& w: neuron.weight)
                f >> w;
            
            neuron.last_weight.resize(size);
            for(T& w: neuron.last_weight)
                f >> w;
        }
    }
    
    return true;
}
