#!python3

json_string = """
{
  "string": "Hello, World!",
  "number": 42,
  "boolean": true,
  "nullValue": null,
  "object": {
    "key1": "value1",
    "key2": 100,
    "nestedObject": {
      "nestedKey": "nestedValue"
    }
  },
  "array": [
    "item1",
    2,
    false,
    null,
    {
      "arrayObjectKey": "arrayObjectValue"
    }
  ]
}
"""

with open("jsons/big_gen.json", "w") as file:
    file.write("{\n")
    file.write("\"o{}\":".format(0))
    file.write(json_string)

    for i in range(1, 40000):
        file.write(",\n")
        file.write("\"o{}\":".format(i))
        file.write(json_string)

    file.write("}\n")
