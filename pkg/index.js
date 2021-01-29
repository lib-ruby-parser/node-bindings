switch (process.platform) {
    case 'linux':
        module.exports = require('./linux.node');
        break;
    case 'darwin':
        module.exports = require('./darwin.node')
        break;
    case 'win32':
        module.exports = require('./win32.node');
        break;
    default:
        throw new Error(`unsupported process.platform '${process.platform}' (only 'linux', 'darwin' and 'win32' are supported)`);
}
