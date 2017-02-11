$(document).ready(function(){
	$('#addClassButton').click(function(){
		var classNameInput = $("#classSearchTextId").val();
		$('#classListId').append('<li><p>' + classNameInput + "</p></li>");
	});
});
