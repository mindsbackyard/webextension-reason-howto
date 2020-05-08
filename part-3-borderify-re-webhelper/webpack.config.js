const path = require('path');

module.exports = {
    entry: './src/borderify.bs.js',
    output: {
        filename: 'borderify.packed.js',
        path: path.resolve(__dirname, 'dist'),
    },
    mode: 'production'
};