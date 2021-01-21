switch (process.platform) {
    case 'linux':
        module.exports = require('./linux-x86_64.node');
        break;
    case 'darwin':
        module.exports = require('./darwin-x86_64.node')
        break;
    default:
        throw new Error(`unsupported process.platform '${process.platform}' (only 'linux' and 'darwin' are supported)`);
}
