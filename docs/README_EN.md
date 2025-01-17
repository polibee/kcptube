# KCP Tube

## Modes

### Supported Modes
Currently 3 modes are supported:
- Client Mode
- Servers Mode
- Relay Mode

## Usage

### Command
`kcptube config.conf`
or
`kcptube config1.conf config2.conf`
If there are multiple files, you can then add them.

If you want to test connectivity before establish connection, just add ``--try`` option

```
kcptube --try config1.conf
```
or
```
kcptube config1.conf --try
```

#### Examples of Client and Server Mode
Please refer [General Configuration](client_server_en.md) introduction page

#### Example of Realy Mode
Please refer [Relay Mode Configuration](relay_mode_en.md) introduction page

## Parameters
Please refer [parameter list](parameters_en.md)

## Configuration File Generator

You can generate a configuration file by using [KCPTube Generator](https://github.com/cnbatch/KCPTubeGenerator)

## Specific examples
Please refer [specific examples](specific_examples_en.md)