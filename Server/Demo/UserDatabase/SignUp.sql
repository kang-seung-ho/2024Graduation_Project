CREATE PROCEDURE [dbo].[SignUp]
	@id char(16),
	@pw char(32)
AS
	SELECT @id, @pw;

RETURN 0
